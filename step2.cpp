#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Config.hpp"
#include "util/util.hpp"

using Symbol = std::string;
using SymbolSet = std::set<Symbol>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

namespace {

    void writeCSV(const std::set<ProductSet>& res, const Config& cfg) {
        auto csv = util::createFile(cfg.toPath());
        for (const auto& ps : res) {
            std::set<std::string> row;
            for (const auto& p : ps) {
                std::vector<std::string> parts;
                for (const auto& ss : p)
                    parts.push_back(util::join(ss, "-"));
                row.insert(util::join(parts, ","));
            }
            csv << util::join(row, ",") << std::endl;
        }
        std::cout << cfg.toPath() << " Saved." << std::endl;
    }

} // namespace

int main() {
    const Config cfg("config.txt", "output/step1", "output/step2");

    const auto data = util::readCSV(cfg.toPath(true));

    std::set<ProductSet> result;
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
        result.insert(std::move(ps));
    }

    writeCSV(result, cfg);

    return 0;
}
