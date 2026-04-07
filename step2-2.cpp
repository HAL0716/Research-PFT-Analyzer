#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Analysis/Engine.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "util/util.hpp"
#include "Transform.hpp"

namespace {

    auto readCSV(const Config& cfg) {
        const auto csv = util::readCSV(cfg.toPath("step1"));
        std::vector<ProductSet> result;

        if (csv.empty())
            return result;

        for (const auto& row : csv)
            result.push_back(Transform::toProductSet(row, cfg));

        return result;
    }

    void writeCSV(const std::vector<ProductSet>& res, const Config& cfg) {
        const auto alpha = Alphabet(cfg.Q);
        auto analyzer = Analysis::Engine(cfg, alpha);

        const auto csvPath = cfg.toPath("step2-2");
        auto csv = util::createFile(csvPath);
        size_t cnt = 0, total = res.size();
        for (const auto& ps : res) {
            Logger::progress(++cnt, total, "Analysis: ", true);

            analyzer.set(ps);
            std::vector<std::string> resultRow;
            for (const auto& b : analyzer.getResult())
                resultRow.push_back(b ? "T" : "F");

            csv << util::join(resultRow, ",") << std::endl;
        }
        std::cout << csvPath << " Saved." << std::endl;
    }

} // namespace

int main() {
    const Config base("config.txt");

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = 1; N <= maxN; ++N) {
        if (N < base.P)
            continue;

        const auto cfg = base.withN(N);
        auto data = readCSV(cfg);

        if (!data.empty())
            writeCSV(data, cfg);
    }

    return 0;
}
