#include <filesystem>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    constexpr std::string MASK_TOKEN = "*";

    struct Record {
        std::vector<std::string> features;
        util::ull vertNum;
        util::ull maskSize;

        bool operator==(const Record& other) const {
            return features == other.features && vertNum == other.vertNum;
        }

        bool operator<(const Record& other) const {
            if (features != other.features)
                return features < other.features;
            return vertNum < other.vertNum;
        }
    };

    template <typename Func>
    void processRows(const util::csvData& row1, const util::csvData& row2, std::vector<Record>& res, Func genVertNum) {
        for (size_t i = 0; i < row1.size(); ++i) {
            util::checkInterrupted();

            const auto vertNum = genVertNum(row1[i]);

            res.push_back(Record{row2[i], vertNum, vertNum});
        }
    }

    std::string makeKey(const Record& r, int mask, int featureCount) {
        std::vector<std::string> key;
        key.reserve(featureCount);
        for (int i = 0; i < featureCount; ++i)
            key.push_back((mask & (1 << i)) ? r.features[i] : MASK_TOKEN);
        return util::join(key, ",");
    }

    void writeOutput(const std::vector<std::string>& res, std::ostream& out) {
        out << util::join(res, "\n");
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step2-1")) || !std::filesystem::exists(cfg.toPath("step2-2"));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    std::vector<Record> data;

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        Logger::progress(N, maxN, "Analyzing N = " + std::to_string(N) + ": ", true);

        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg))
            continue;

        const auto row1 = util::readCSV(cfg.toPath("step2-1"));
        const auto row2 = util::readCSV(cfg.toPath("step2-2"));
        if (row1.size() != row2.size())
            continue;

        const size_t split = cfg.L / cfg.T + 1;
        auto genVertNum = [split](const std::vector<std::string>& row) -> util::ull {
            return std::accumulate(row.begin() + split, row.end(), static_cast<util::ull>(0), [](util::ull acc, const std::string& s) { return acc + std::stoull(s); });
        };

        try {
            processRows(row1, row2, data, genVertNum);
            util::normalize(data);
        } catch (const std::exception& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    std::vector<size_t> masks;
    for (size_t i = 0; i < baseConfig.featureNum; ++i)
        masks.push_back(i);

    std::vector<std::string> res;

    for (size_t i = 0; i < masks.size(); ++i) {
        Logger::progress(i + 1, masks.size(), "Analyzing ", true);

        const auto bitCount = util::popcount(masks[i]);
        std::unordered_map<std::string, std::vector<size_t>> groups;

        for (size_t j = 0; j < data.size(); ++j)
            if (data[j].maskSize >= bitCount)
                groups[makeKey(data[j], masks[i], baseConfig.featureNum)].push_back(j);

        for (auto& [key, indices] : groups) {
            std::set<size_t> nums;
            for (const auto idx : indices)
                nums.insert(data[idx].vertNum);

            if (nums.size() == 1 && *nums.begin() == baseConfig.targetNum) {
                res.push_back(key + " : " + std::to_string(*nums.begin()));

                for (const auto idx : indices)
                    data[idx].maskSize = std::min(data[idx].maskSize, bitCount);
            }
        }
    }

    util::SafeOutput out(baseConfig.toPath("step3-3", false));

    writeOutput(res, out.stream());
    out.commit();

    return 0;
}
