#include <iostream>
#include <map>
#include <string>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    using recordMap = std::map<std::string, std::string>;

    void analyze(const std::string& label, const util::csvData& data, recordMap& res) {
        for (size_t i = 0; i < data.size(); ++i) {
            const auto key = util::join(data[i], ",");
            res.emplace(key, label + "_" + std::to_string(i + 1));
        }
    }

    util::csvData format(const recordMap& data) {
        util::csvData res;
        for (const auto& [key, value] : data)
            res.push_back({key, value});
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

        analyze("N=" + std::to_string(N), data, res);
    }

    util::writeCSV(base.toPath("step3-1", false), format(res));

    return 0;
}
