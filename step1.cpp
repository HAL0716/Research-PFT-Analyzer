#include <filesystem>
#include <iostream>
#include <set>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Transform.hpp"
#include "Types.hpp"
#include "Validator/Engine.hpp"
#include "util/util.hpp"

namespace {

    SymbolSet genSymbols(const Config& cfg, const Alphabet& alpha) {
        SymbolSet symbols;

        const size_t limit = util::calcPower(alpha.size(), cfg.T);
        for (size_t i = 0; i < limit; ++i)
            symbols.insert(alpha.toSymbol(i, cfg.T));

        return symbols;
    }

    auto genBluePrint(const Config& cfg, const SymbolSet& symbols) {
        auto calcSum = [](const std::vector<std::vector<size_t>>& group) -> util::ull {
            util::ull sum = 0;
            for (const auto& pattern : group)
                sum += util::calcProduct(pattern);
            return sum;
        };

        const auto indices = util::range(1, symbols.size());
        const auto perms = util::Combinatorics::perms_r(indices, cfg.L / cfg.T);

        std::set<decltype(perms)::value_type> filteredPerms;
        for (const auto& p : perms)
            if (!cfg.FILTER || (p.front() != symbols.size() && p.back() != symbols.size()))
                filteredPerms.insert(p);

        const auto groups = util::Combinatorics::combs_r(filteredPerms, cfg.P);

        std::set<decltype(groups)::value_type> res;

        for (const auto& g : groups)
            if (calcSum(g) == cfg.N)
                res.insert(g);

        return res;
    }

    auto genProductSet(const Config& cfg, const SymbolSet& symbols, const Validator::Engine& isValid, std::ostream& out) {
        const auto base = genBluePrint(cfg, symbols);
        if (base.empty())
            return;

        size_t cnt = 0;
        const size_t total = base.size();
        const std::string label = "Generating N = " + std::to_string(cfg.N) + ": ";

        for (const auto& group : base) {
            util::checkInterrupted();

            Logger::progress(++cnt, total, label, true);

            std::vector<ProductSet> candidates;
            for (const auto& pattern : group) {
                util::checkInterrupted();

                std::vector<std::set<SymbolSet>> combs;
                for (auto n : pattern) {
                    util::checkInterrupted();

                    combs.push_back(util::Combinatorics::combs(symbols, n));
                }

                candidates.push_back(util::Product::asVec(combs));
            }

            for (const auto& ps : util::Product::asSet(candidates))
                if (isValid(ps)) {
                    util::checkInterrupted();
                    out << util::join(Transform::toCsvRow(ps, cfg), ",") << "\n";
                }
        }
    }

    bool shouldSkip(const Config& cfg) {
        return std::filesystem::exists(cfg.toPath("step1")) && !cfg.UPDATE;
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    const Alphabet alpha(baseConfig.Q);
    const SymbolSet symbols = genSymbols(baseConfig, alpha);

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg))
            continue;

        util::SafeOutput out(cfg.toPath("step1"));

        try {
            const Validator::Engine validate(cfg, alpha, symbols);
            genProductSet(cfg, symbols, validate, out.stream());

            out.commit();
        } catch (const std::exception& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    return 0;
}
