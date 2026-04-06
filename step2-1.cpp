#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Graph.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "util/util.hpp"

namespace {
    SymbolSet expand(const Product& p) {
        SymbolSet res{""};

        for (const auto& ss : p) {
            SymbolSet next;
            for (const auto& prefix : res)
                for (const auto& s : ss)
                    next.insert(prefix + s);
            res.swap(next);
        }

        return res;
    }

    SymbolSet toWords(const ProductSet& ps) {
        SymbolSet words;

        for (const auto& p : ps) {
            auto expanded = expand(p);
            words.insert(expanded.begin(), expanded.end());
        }

        return words;
    }

    auto readCSV(const Config& cfg) {
        const auto csv = util::readCSV(cfg.toPath("step1"));
        std::vector<ProductSet> result;

        if (csv.empty())
            return result;

        for (const auto& row : csv) {
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
            result.push_back(std::move(ps));
        }

        return result;
    }

    void writeCSV(const std::vector<ProductSet>& res, const Config& cfg) {
        const auto alpha = Alphabet(cfg.Q);
        auto graph = Graph(cfg, alpha);

        const auto csvPath = cfg.toPath("step2-1");
        auto csv = util::createFile(csvPath);
        size_t cnt = 0, total = res.size();
        for (const auto& ps : res) {
            Logger::progress(++cnt, total, "Graph Generation: ", true);

            std::vector<std::string> resultRow;

            graph.set(toWords(ps));
            resultRow.push_back(std::to_string(graph.getV().size()));
            graph.minimize();
            resultRow.push_back(std::to_string(graph.getV().size()));

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
