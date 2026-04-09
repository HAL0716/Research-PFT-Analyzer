#include <map>
#include <set>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    using recordKey = std::pair<size_t, std::string>;
    using recordMap = std::map<recordKey, std::set<std::string>>;

    void analyze(const util::csvData& data, const Config& cfg, recordMap& res) {
        const size_t split = cfg.L / cfg.T + 1;

        for (const auto& row : data) {
            std::vector<std::string> first(row.begin(), row.begin() + split);
            std::vector<std::string> second(row.begin() + split, row.end());

            auto key = recordKey{cfg.N, util::join(first, "-")};
            auto [it, inserted] = res.try_emplace(key);
            it->second.insert(util::join(second, "-"));
        }
    }

    util::csvData format(const recordMap& data) {
        util::csvData res;
        for (const auto& [key, value] : data) {
            std::vector<std::string> row = {key.second};
            for (const auto& v : value)
                row.push_back(v);
            res.push_back(row);
        }
        return res;
    }

} // namespace

int main() {
    const Config base("config.txt");

    recordMap res;

    const size_t maxN = util::calcPower(base.Q, base.L);

    for (size_t N = base.P; N <= maxN; ++N) {
        Logger::progress(N, maxN, "Analyzing N: ", true);

        const auto cfg = base.withN(N);
        const auto data = util::readCSV(cfg.toPath("step2-1"));

        if (data.empty())
            continue;

        analyze(data, cfg, res);
    }

    util::writeCSV(base.toPath("step3-1", false), format(res));

    return 0;
}
