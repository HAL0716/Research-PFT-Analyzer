#include <filesystem>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Analysis/Engine.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Transform.hpp"
#include "Types.hpp"
#include "util/util.hpp"

namespace {

    auto parseCSV(const util::csvData& data, const Config& cfg) {
        std::vector<ProductSet> res;
        for (const auto& row : data)
            res.push_back(Transform::toProductSet(row, cfg));
        return res;
    }

    auto analyze(const std::vector<ProductSet>& data, const Config& cfg) {
        auto format = [](const std::vector<bool>& result) -> std::string {
            std::vector<std::string> res;
            for (bool b : result)
                res.push_back(b ? "T" : "F");
            return util::join(res, ",");
        };

        const auto alpha = Alphabet(cfg.Q);
        auto analyzer = Analysis::Engine(cfg, alpha);

        util::csvData res;

        size_t cnt = 0, total = data.size();
        for (const auto& ps : data) {
            Logger::progress(++cnt, total, "Processing: ", true);

            std::vector<std::string> resRow;

            analyzer.set(ps);
            resRow.push_back(format(analyzer.getResult()));

            res.push_back(std::move(resRow));
        }
        return res;
    }

} // namespace

int main() {
    const bool UPDATE = false;

    const Config base("config.txt");

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = 1; N <= maxN; ++N) {
        if (N < base.P)
            continue;

        const auto cfg = base.withN(N);

        if (std::filesystem::exists(cfg.toPath("step2-2")) && !UPDATE)
            continue;

        const auto csvRaw = util::readCSV(cfg.toPath("step1"));
        if (csvRaw.empty())
            continue;

        const auto csvParsed = parseCSV(csvRaw, cfg);

        const auto res = analyze(csvParsed, cfg);

        util::writeCSV(cfg.toPath("step2-2"), res);
    }

    return 0;
}
