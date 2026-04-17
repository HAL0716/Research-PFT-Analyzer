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

    using recordMap = std::map<std::string, std::map<std::string, std::set<std::string>>>;

    void processRows(const util::csvData& row1, const util::csvData& row2, recordMap& res, const Config& cfg) {
        const size_t split = cfg.L / cfg.T + 1;
        auto makeKeys = [split](const std::vector<std::string>& row) {
            std::vector<std::string> key1(row.begin(), row.begin() + split);
            std::vector<std::string> key2(row.begin() + split, row.end());
            return std::make_pair(util::join(key1, "-"), util::join(key2, "-"));
        };

        for (size_t i = 0; i < row1.size(); ++i) {
            util::checkInterrupted();

            const auto [k1, k2] = makeKeys(row1[i]);
            const auto value = util::join(row2[i], "-");

            res[value][k2].insert(k1);
        }
    }

    void writeOutput(const recordMap& res, std::ostream& out) {
        for (const auto& [key1, submap] : res) {
            out << key1 << '\n';
            for (const auto& [key2, values] : submap) {
                out << '\t' << key2 << '\n';
                out << '\t' << '\t' << util::join(values, ",") << '\n';
            }
        }
        out << '\n';
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step2-1")) || !std::filesystem::exists(cfg.toPath("step2-2"));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    recordMap res;

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        Logger::progress(N, maxN, "Analyzing N = " + std::to_string(N) + ": ", true);

        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg))
            continue;

        const auto row1 = util::readCSV(cfg.toPath("step2-1"));
        const auto row2 = util::readCSV(cfg.toPath("step2-2"));
        if (row1.size() != row2.size())
            continue;

        processRows(row1, row2, res, cfg);
    }

    util::SafeOutput out(baseConfig.toPath("step3-2", false));

    writeOutput(res, out.stream());
    out.commit();

    return 0;
}
