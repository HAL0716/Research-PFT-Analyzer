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

    void writeOutput(const std::vector<Record>& res, std::ostream& out) {
        for (const auto& record : res) {
            out << util::join(record.features, ",") << ',' << record.vertNum << '\n';
        }
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step2-1")) || !std::filesystem::exists(cfg.toPath("step2-2"));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    std::vector<Record> res;

    for (size_t N = baseConfig.P; N <= 10; ++N) {
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
            processRows(row1, row2, res, genVertNum);
            util::normalize(res);
        } catch (const std::exception& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    util::SafeOutput out(baseConfig.toPath("step3-3", false));

    writeOutput(res, out.stream());
    out.commit();

    return 0;
}
