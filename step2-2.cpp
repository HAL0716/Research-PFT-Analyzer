#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Analysis/Engine.hpp"
#include "Analyzer.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "util/util.hpp"

namespace {

    auto readCSV(const Config& cfg) {
        const auto csv = util::readCSV(cfg.toPath(true));
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
        auto analyzer = Analysis::Engine(cfg, alpha);

        auto csv = util::createFile(cfg.toPath());
        size_t cnt = 0, total = res.size();
        for (const auto& ps : res) {
            Logger::progress(++cnt, total, "Graph Generation: ", true);

            std::vector<std::string> row;

            analyzer.set(ps);
            std::vector<std::string> res;
            for (const auto& b : analyzer.getResult())
                res.push_back(b ? "1" : "0");
            csv << util::join(res, ",") << std::endl;
        }
        std::cout << cfg.toPath() << " Saved." << std::endl;
    }

} // namespace

int main() {
    const Config base("step1", "step2-2");

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
