#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

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

        for (const auto& row : data) {
            if (row.size() != cfg.L / cfg.T * cfg.P)
                throw std::runtime_error("invalid row size");

            ProductSet ps;
            for (size_t i = 0; i < row.size(); i += cfg.L / cfg.T) {
                Product p;
                for (size_t j = 0; j < cfg.L / cfg.T; ++j) {
                    SymbolSet ss;
                    std::stringstream ssStream(row[i + j]);
                    std::string sym;

                    while (std::getline(ssStream, sym, '-'))
                        ss.insert(sym);

                    p.push_back(std::move(ss));
                }
                ps.insert(std::move(p));
            }
            res.push_back(std::move(ps));
        }
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
    const Config base("config.txt");

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = 1; N <= maxN; ++N) {
        if (N < base.P)
            continue;

        const auto cfg = base.withN(N);

        const auto csvRaw = util::readCSV(cfg.toPath("step1"));
        if (csvRaw.empty())
            continue;

        const auto csvParsed = parseCSV(csvRaw, cfg);

        const auto res = analyze(csvParsed, cfg);

        util::writeCSV(cfg.toPath("step2-1"), res);
    }

    return 0;
}
