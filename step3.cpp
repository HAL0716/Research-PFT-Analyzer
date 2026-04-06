#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    using recordMap = std::unordered_map<std::string, std::set<size_t>>;

    auto readCSV(const Config& cfg, recordMap& result) {
        const auto csv1 = util::readCSV(cfg.toPath("step2-1"));
        const auto csv2 = util::readCSV(cfg.toPath("step2-2"));

        if (csv1.empty() || csv2.empty() || csv1.size() != csv2.size())
            return result;

        for (size_t i = 0; i < csv1.size(); ++i) {
            const auto& row1 = csv1[i];
            const auto& row2 = csv2[i];

            const auto key = util::join(row2, ",");
            const auto value = std::stoul(row1[1]);

            result[key].insert(value);
        }

        return result;
    }

    void writeCSV(const recordMap& res, const Config& cfg) {
        const auto csvPath = cfg.toPath("step3", false);
        auto csv = util::createFile(csvPath);
        size_t cnt = 0, total = res.size();
        for (const auto& pair : res) {
            Logger::progress(++cnt, total, "Processing: ", true);

            std::vector<std::string> resultRow;
            resultRow.push_back(pair.first);
            resultRow.push_back(util::join(pair.second, ","));

            csv << util::join(resultRow, ",") << std::endl;
        }
        std::cout << csvPath << " Saved." << std::endl;
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
        readCSV(cfg, data);
    }

    if (!data.empty())
        writeCSV(data, base);

    return 0;
}
