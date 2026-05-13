#include <algorithm>
#include <ostream>
#include <span>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    struct Record {
        std::vector<std::vector<std::string>> features;
        std::string vertNums;
        size_t maskSize = std::numeric_limits<size_t>::max();

        std::vector<std::string> masked(int mask) const {
            std::vector<std::string> res;
            for (const auto& feat : features) {
                std::vector<std::string> key;
                key.reserve(feat.size());
                for (size_t i = 0; i < feat.size(); ++i)
                    key.push_back((mask & (1 << i)) ? feat[i] : "*");
                res.push_back(util::join(key, ","));
            }
            return res;
        }

        bool canUse(int mask) const {
            return util::popcount(mask) <= maskSize;
        }

        void update(size_t bitCount) {
            maskSize = std::min(maskSize, bitCount);
        }

        bool operator==(const Record& other) const {
            return std::tie(features, vertNums, maskSize) == std::tie(other.features, other.vertNums, other.maskSize);
        }

        bool operator<(const Record& other) const {
            return std::tie(features, vertNums, maskSize) < std::tie(other.features, other.vertNums, other.maskSize);
        }

        struct Hash {
            size_t operator()(const Record& r) const {
                size_t h = 0;
                for (const auto& feat : r.features)
                    h ^= std::hash<std::string>()(util::join(feat, ",")) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<std::string>()(r.vertNums) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<size_t>()(r.maskSize) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
    };

    using RecordSet = std::unordered_set<Record, Record::Hash>;

    void updateRecords(const util::csvData& data, RecordSet& records) {
        size_t cnt = 0;
        const size_t total = data.size();

        for (const auto& row : data) {
            Logger::progress(++cnt, total, "Loading : ", true);

            if (row.size() < 2)
                throw std::runtime_error("Invalid row: " + util::join(row, ","));

            std::vector<std::vector<std::string>> features;
            for (const auto& feat : util::split(row[0], ':'))
                features.push_back(util::split(feat, '-'));
            std::sort(features.begin(), features.end());

            std::string vertNums = row[1];

            records.emplace(Record{features, vertNums});
        }
    }

    struct Analyzer {
      public:
        Analyzer(const RecordSet& r) : records(r.begin(), r.end()) {
            if (records.empty())
                throw std::invalid_argument("No records to analyze");
        }

        std::set<util::csvRow> run() {
            std::set<util::csvRow> res;
            size_t total = 1 << records[0].features[0].size();
            for (size_t mask = 0; mask < total; ++mask) {
                Logger::progress(mask + 1, total, "Masking : ", true);

                for (const auto& [_, indices] : groupByMask(mask)) {
                    const auto nums = toNums(indices);
                    if (nums.size() != 1)
                        continue;

                    const auto [key, matchCount] = toKey(indices);
                    if (matchCount != util::popcount(mask))
                        continue;

                    res.insert(toRow(key, nums, mask, indices));
                    updateRecords(indices, mask);
                }
            }
            appendUnresolved(res, total);
            return res;
        }

      private:
        std::vector<Record> records;

        using Index = std::pair<size_t, size_t>; // (record index, feature index)

        std::unordered_map<std::string, std::set<Index>> groupByMask(size_t mask) {
            std::unordered_map<std::string, std::pair<std::set<Index>, bool>> tmp;
            for (size_t i = 0; i < records.size(); ++i) {
                const auto& masked = records[i].masked(mask);
                for (size_t j = 0; j < masked.size(); ++j) {
                    auto& [indices, ok] = tmp[masked[j]];
                    indices.insert({i, j});
                    ok |= records[i].canUse(mask);
                }
            }

            std::unordered_map<std::string, std::set<Index>> res;
            for (const auto& [k, v] : tmp)
                if (v.second)
                    res.emplace(k, std::move(v.first));
            return res;
        }

        std::set<std::string> toNums(const std::set<Index>& indices) {
            std::set<std::string> res;
            for (auto idx : indices)
                res.insert(records[idx.first].vertNums);
            return res;
        }

        std::pair<std::vector<std::string>, size_t> toKey(const std::set<Index>& indices) {
            const auto& baseIdx = indices.begin();
            const auto& base = records[baseIdx->first].features[baseIdx->second];

            auto key = buildKey(indices, base);
            size_t matchCount = countMatches(key);

            return {key, matchCount};
        }

        std::vector<std::string> buildKey(const std::set<Index>& indices, const std::vector<std::string>& base) {
            std::vector<std::string> key = base;
            for (auto idx : indices) {
                const auto& features = records[idx.first].features[idx.second];
                for (size_t i = 0; i < key.size(); ++i)
                    if (key[i] != features[i])
                        key[i] = "*";
            }
            return key;
        }

        size_t countMatches(const std::vector<std::string>& key) {
            return std::count_if(key.begin(), key.end(), [](const std::string& v) { return v != "*"; });
        }

        util::csvRow toRow(const std::vector<std::string>& key, const std::set<std::string>& nums, size_t mask, const std::set<Index>& indices) {
            util::csvRow row;
            row.reserve(3 + indices.size());
            row.push_back(util::join(key, ","));
            row.push_back(util::join(nums, ","));
            row.push_back(std::to_string(mask));
            for (auto idx : indices)
                row.push_back(util::join(records[idx.first].features[idx.second], "-"));
            return row;
        }

        void updateRecords(const std::set<Index>& indices, size_t mask) {
            size_t bit = util::popcount(mask);
            for (auto idx : indices)
                records[idx.first].update(bit);
        }

        void appendUnresolved(std::set<util::csvRow>& res, size_t total) {
            for (const auto& record : records)
                if (record.maskSize == std::numeric_limits<size_t>::max())
                    for (const auto& feat : record.features)
                        res.insert({util::join(feat, ","), record.vertNums, std::to_string(total - 1)});
        }
    };

    void writeOutput(const std::set<util::csvRow>& result, util::SafeOutput& out) {
        util::csvData sorted(result.begin(), result.end());
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
            auto key = [](const auto& v) { return std::tuple{v[1], std::stoul(v[2]), v[0]}; };
            return key(a) < key(b);
        });

        for (const auto& row : sorted)
            out.stream() << util::join(row, ",") << "\n";
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step4", false));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    Config cfg("config.txt");

    util::SafeOutput out(cfg.toPath("step5", false));

    if (shouldSkip(cfg))
        return 0;

    const auto data = util::readCSV(cfg.toPath("step4", false));

    RecordSet records;
    updateRecords(data, records);
    std::cout << "Loaded " << records.size() << " unique records\n";

    Analyzer analyzer(records);
    const auto result = analyzer.run();

    writeOutput(result, out);
    out.commit();

    return 0;
}
