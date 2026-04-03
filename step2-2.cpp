#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Analyzer.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

using Symbol = std::string;
using SymbolSet = std::set<Symbol>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

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
        const auto csv = util::readCSV(cfg.toPath(true));
        std::vector<ProductSet> result;
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
        const auto analyzer = Analyzer(cfg, alpha);

        auto csv = util::createFile(cfg.toPath());
        size_t cnt = 0, total = res.size();
        for (const auto& ps : res) {
            Logger::progress(++cnt, total, "Graph Generation: ", true);

            std::vector<std::string> row;

            analyzer.run(toWords(ps));
        }
        std::cout << cfg.toPath() << " Saved." << std::endl;
    }

} // namespace

int main() {
    const Config base("step1", "step2");

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = 1; N <= maxN; ++N) {
        if (N < base.P)
            continue;

        const auto cfg = base.withN(N);
        auto data = readCSV(cfg);

        const auto ps = data[0]; // 仮に最初の行だけ処理

        break; // 実装中のため
    }

    return 0;
}
