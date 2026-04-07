#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Graph.hpp"
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
        auto format = [](const Graph::Verts& verts, const Config& cfg) -> std::string {
            std::vector<size_t> res(cfg.L / cfg.T + 1, 0);
            for (const auto& s : verts) {
                size_t pos = s.find('+');
                if (pos == std::string::npos)
                    pos = s.size();
                res[pos / cfg.T]++;
            }
            return util::join(res, ",");
        };

        const auto alpha = Alphabet(cfg.Q);
        auto graph = Graph(cfg, alpha);

        util::csvData res;

        size_t cnt = 0, total = data.size();
        for (const auto& ps : data) {
            Logger::progress(++cnt, total, "Processing: ", true);

            std::vector<std::string> resRow;

            graph.set(Transform::toWords(ps));
            resRow.push_back(format(graph.getV(), cfg));
            graph.minimize();
            resRow.push_back(format(graph.getV(), cfg));

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

        if (std::filesystem::exists(cfg.toPath("step2-1")) && !UPDATE)
            continue;

        const auto csvRaw = util::readCSV(cfg.toPath("step1"));
        if (csvRaw.empty())
            continue;

        const auto csvParsed = parseCSV(csvRaw, cfg);

        const auto res = analyze(csvParsed, cfg);

        util::writeCSV(cfg.toPath("step2-1"), res);
    }

    return 0;
}
