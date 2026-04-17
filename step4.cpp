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
        std::vector<std::string> features;
        std::string vertNums;
        size_t maskSize;

        std::string masked(int mask) const {
            std::vector<std::string> key;
            key.reserve(features.size());
            for (size_t i = 0; i < features.size(); ++i)
                key.push_back((mask & (1 << i)) ? features[i] : "*");
            return util::join(key, ",");
        }

        bool canUse(size_t bitCount) const {
            return maskSize >= bitCount;
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

        static Record make(const util::csvRow& data) {
            if (data.size() != 3)
                throw std::invalid_argument("Invalid data size");

            const auto& vert = data[1];
            const auto& feat = util::split(data[2]);

            return Record{feat, vert, feat.size()};
        }

        struct Hash {
            size_t operator()(const Record& r) const {
                size_t h = 0;
                h ^= std::hash<std::string>()(util::join(r.features, ",")) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<std::string>()(r.vertNums) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<size_t>()(r.maskSize) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
    };

    using RecordSet = std::unordered_set<Record, Record::Hash>;

    void updateRecords(const util::csvData& data, RecordSet& records) {
        for (size_t i = 0; i < data.size(); ++i) {
            util::checkInterrupted();

            Logger::progress(i + 1, data.size(), "Loading : ", true);

            records.insert(Record::make(data[i]));
        }
    }

    struct Analyzer {
      public:
        Analyzer(const RecordSet& r) : records(r.begin(), r.end()) {
            if (records.empty())
                throw std::invalid_argument("No records to analyze");
        }

        util::csvData run() {
            util::csvData res;
            size_t total = 1 << records[0].features.size();
            for (size_t mask = 0; mask < total; ++mask) {
                Logger::progress(mask + 1, total, "Masking ", true);

                for (const auto& [key, indices] : groupByMask(mask)) {
                    const auto nums = toNums(indices);
                    if (nums.size() != 1)
                        continue;
                    res.push_back({key, *nums.begin()});
                    updateRecords(indices, mask);
                }
            }
            return res;
        }

      private:
        std::vector<Record> records;

        std::unordered_map<std::string, std::set<size_t>> groupByMask(size_t mask) {
            std::unordered_map<std::string, std::set<size_t>> res;
            size_t bit = util::popcount(mask);
            for (size_t i = 0; i < records.size(); ++i)
                if (records[i].canUse(bit))
                    res[records[i].masked(mask)].insert(i);
            return res;
        }

        std::set<std::string> toNums(const std::set<size_t>& indices) {
            std::set<std::string> res;
            for (auto idx : indices)
                res.insert(records[idx].vertNums);
            return res;
        }

        void updateRecords(const std::set<size_t>& indices, size_t mask) {
            size_t bit = util::popcount(mask);
            for (auto idx : indices)
                records[idx].update(bit);
        }
    };

    void writeOutput(const util::csvData& result, util::SafeOutput& out) {
        util::csvData sorted(result.begin(), result.end());
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
            return (a[1] == b[1]) ? a[0] > b[0] : a[1] < b[1];
        });

        for (const auto& row : sorted)
            out.stream() << util::join(row, ",") << "\n";
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step3", false));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    Config cfg("config.txt");
    if (shouldSkip(cfg))
        return 0;

    const auto data = util::readCSV(cfg.toPath("step3", false));

    RecordSet records;
    updateRecords(data, records);
    std::cout << "Loaded " << records.size() << " unique records\n";

    Analyzer analyzer(records);
    const auto result = analyzer.run();

    util::SafeOutput out(cfg.toPath("step4", false));
    writeOutput(result, out);
    out.commit();

    return 0;
}
