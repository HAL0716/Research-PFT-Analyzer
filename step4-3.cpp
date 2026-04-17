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

    struct Record {
        std::vector<std::string> features;
        size_t vertNum;
        size_t maskSize;

        std::string makeKey(int mask, int featureCount) const {
            std::vector<std::string> key;
            key.reserve(featureCount);
            for (int i = 0; i < featureCount; ++i)
                key.push_back((mask & (1 << i)) ? features[i] : "*");
            return util::join(key, ",");
        }

        bool canUse(size_t bitCount) const {
            return maskSize >= bitCount;
        }

        void updateMask(size_t bitCount) {
            maskSize = std::min(maskSize, bitCount);
        }

        bool operator==(const Record& other) const {
            return features == other.features && vertNum == other.vertNum && maskSize == other.maskSize;
        }

        bool operator<(const Record& other) const {
            return std::tie(features, vertNum, maskSize) < std::tie(other.features, other.vertNum, other.maskSize);
        }
    };

    class DataBuilder {
      public:
        explicit DataBuilder(const Config& base) : baseConfig(base) {
        }

        std::vector<Record> build() {
            std::vector<Record> data;
            const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

            for (size_t N = baseConfig.P; N <= maxN; ++N) {
                Logger::progress(N, maxN, "Analyzing N = " + std::to_string(N) + ": ", true);

                auto cfg = baseConfig.withN(N);
                if (shouldSkip(cfg))
                    continue;

                auto row1 = util::readCSV(cfg.toPath("step2-1"));
                auto row2 = util::readCSV(cfg.toPath("step2-2"));
                if (row1.size() != row2.size())
                    continue;

                process(row1, row2, data, cfg);
                util::normalize(data);
            }
            return data;
        }

      private:
        const Config& baseConfig;

        static bool shouldSkip(const Config& cfg) {
            return !std::filesystem::exists(cfg.toPath("step2-1")) || !std::filesystem::exists(cfg.toPath("step2-2"));
        }

        static void process(const util::csvData& row1, const util::csvData& row2, std::vector<Record>& out, const Config& cfg) {
            const size_t split = cfg.L / cfg.T + 1;

            for (size_t i = 0; i < row1.size(); ++i) {
                util::checkInterrupted();

                auto vertNum = static_cast<size_t>(std::accumulate(
                    row1[i].begin() + split, row1[i].end(),
                    static_cast<util::ull>(0),
                    [](util::ull acc, const std::string& s) {
                        return acc + std::stoull(s);
                    }));

                out.push_back({row2[i], vertNum, vertNum});
            }
        }
    };

    class Analyzer {
      public:
        Analyzer(std::vector<Record>& d, const Config& cfg)
            : data(d), config(cfg) {
        }

        std::vector<std::string> run() {
            std::vector<std::string> res;
            size_t total = 1 << config.featureNum;
            for (size_t mask = 0; mask < total; ++mask) {
                Logger::progress(mask + 1, total, "Analyzing ", true);

                auto groups = groupByMask(mask);

                for (auto& [key, indices] : groups) {
                    if (isTargetGroup(indices)) {
                        res.push_back(key + " : " + std::to_string(config.targetNum));
                        updateMask(indices, util::popcount(mask));
                    }
                }
            }
            return res;
        }

      private:
        std::vector<Record>& data;
        const Config& config;

        std::unordered_map<std::string, std::vector<size_t>> groupByMask(size_t mask) {
            std::unordered_map<std::string, std::vector<size_t>> groups;
            size_t bitCount = util::popcount(mask);

            for (size_t i = 0; i < data.size(); ++i) {
                if (!data[i].canUse(bitCount))
                    continue;
                groups[data[i].makeKey(mask, config.featureNum)].push_back(i);
            }
            return groups;
        }

        bool isTargetGroup(const std::vector<size_t>& indices) {
            std::set<size_t> nums;
            for (auto idx : indices)
                nums.insert(data[idx].vertNum);

            return nums.size() == 1 && *nums.begin() == config.targetNum;
        }

        void updateMask(const std::vector<size_t>& indices, size_t bitCount) {
            for (auto idx : indices)
                data[idx].updateMask(bitCount);
        }
    };

} // namespace

int main() {
    util::setupSignalHandler();

    Config config("config.txt");

    DataBuilder builder(config);
    auto data = builder.build();

    Analyzer analyzer(data, config);
    auto result = analyzer.run();

    util::SafeOutput out(config.toPath("step3-3", false));
    out.stream() << util::join(result, "\n");
    out.commit();
}
