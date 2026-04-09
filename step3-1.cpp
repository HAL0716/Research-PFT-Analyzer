#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    using recordMap = std::map<std::string, std::string>;

    auto analyze(const std::string& label, const util::csvData& data, recordMap& res) {
        for (size_t i = 0; i < data.size(); ++i) {
            Logger::progress(i + 1, data.size(), label, true);

            const auto key = util::join(data[i], ",");
            const auto value = label + "_" + std::to_string(i + 1);

            if (res.find(key) == res.end())
                res[key] = value;
        }
        return res;
    }

    auto format(const recordMap& data) {
        util::csvData res;
        for (const auto& [key, value] : data)
            res.push_back({key, value});
        return res;
    }

} // namespace

int main() {
    const Config base("config.txt");

    recordMap data;

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = 1; N <= maxN; ++N) {
        if (N < base.P)
            continue;

        const auto cfg = base.withN(N);

        const auto csvRaw = util::readCSV(cfg.toPath("step2-1"));
        if (csvRaw.empty())
            continue;

        const auto label = "N=" + std::to_string(N);

        analyze(label, csvRaw, data);
    }

    util::writeCSV(base.toPath("step3", false), format(data));

    return 0;
}
