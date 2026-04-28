#include <filesystem>
#include <iostream>
#include <set>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Transform.hpp"
#include "Types.hpp"
#include "util/util.hpp"

namespace {

    SymbolSet genSymbols(const Config& cfg, const Alphabet& alpha) {
        SymbolSet symbols;

        const size_t limit = util::calcPower(alpha.size(), cfg.T);
        for (size_t i = 0; i < limit; ++i)
            symbols.insert(alpha.toSymbol(i, cfg.T));

        return symbols;
    }

    Product applyMap(const Product& p, const Alphabet& alpha, const Symbol& map) {
        Product result;

        for (const auto& symSet : p) {
            SymbolSet mappedSet;

            for (const auto& sym : symSet)
                mappedSet.insert(alpha.add(sym, map));

            result.push_back(mappedSet);
        }

        return result;
    }

    ProductSet applyMap(const ProductSet& ps, const Alphabet& alpha, const Symbol& map) {
        ProductSet out;

        for (const auto& p : ps)
            out.insert(applyMap(p, alpha, map));

        return out;
    }

    class Validator {
      public:
        Validator(const Config& cfg, const Alphabet& alpha, const SymbolSet& symbols)
            : cfg_(cfg), alpha_(alpha), symbols_(symbols) {
        }

        bool operator()(const ProductSet& ps) const {
            return hasCorrectSize(ps) && hasFirstSymbol(ps) && hasNoIntersection(ps) && arePairIndependent(ps) && hasMappingInvariance(ps);
        }

      private:
        const Config& cfg_;
        const Alphabet& alpha_;
        const SymbolSet& symbols_;

        bool hasCorrectSize(const ProductSet& ps) const {
            return ps.size() == cfg_.P;
        }

        bool hasFirstSymbol(const ProductSet& ps) const {
            const Symbol& target = *symbols_.begin();

            for (const auto& p : ps)
                for (const auto& s : p[0])
                    if (s == target)
                        return true;

            return false;
        }

        bool hasNoIntersection(const ProductSet& ps) const {
            if (ps.size() < 2)
                return true;

            auto intersect = [](const Product& a, const Product& b) {
                for (size_t i = 0; i < (a.size() - 1); ++i)
                    if (!util::hasIntersection(a[i], b[i]))
                        return false;
                return true;
            };

            for (auto it1 = ps.begin(); it1 != ps.end(); ++it1)
                for (auto it2 = std::next(it1); it2 != ps.end(); ++it2)
                    if (intersect(*it1, *it2))
                        return false;

            return true;
        }

        bool arePairIndependent(const ProductSet& ps) const {
            if (ps.size() < 2)
                return true;

            auto independent = [](const Product& a, const Product& b) {
                for (size_t skip = 0; skip < a.size(); ++skip) {
                    bool equal = true;
                    for (size_t i = 0; i < a.size(); ++i) {
                        if (i == skip)
                            continue;
                        if (a[i] != b[i]) {
                            equal = false;
                            break;
                        }
                    }

                    if (equal)
                        return false;
                }
                return true;
            };

            for (auto it1 = ps.begin(); it1 != ps.end(); ++it1)
                for (auto it2 = std::next(it1); it2 != ps.end(); ++it2)
                    if (!independent(*it1, *it2))
                        return false;

            return true;
        }

        bool hasMappingInvariance(const ProductSet& ps) const {
            std::set<SymbolSet> mappedWords;
            for (const auto& m : symbols_)
                mappedWords.insert(Transform::toWords(applyMap(ps, alpha_, m)));

            return Transform::toWords(ps) == *mappedWords.begin();
        }
    };

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
            if (p.front() != symbols.size() && p.back() != symbols.size())
                filteredPerms.insert(p);

        const auto groups = util::Combinatorics::combs_r(filteredPerms, cfg.P);

        std::set<decltype(groups)::value_type> res;

        for (const auto& g : groups)
            if (calcSum(g) == cfg.N)
                res.insert(g);

        return res;
    }

    auto genProductSet(const Config& cfg, const SymbolSet& symbols, const Validator& isValid, std::ostream& out) {
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

    bool shouldSkip(const Config& cfg, bool update) {
        return std::filesystem::exists(cfg.toPath("step1")) && !update;
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const bool UPDATE = false;

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    const Alphabet alpha(baseConfig.Q);
    const SymbolSet symbols = genSymbols(baseConfig, alpha);

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg, UPDATE))
            continue;

        util::SafeOutput out(cfg.toPath("step1"));

        try {
            const Validator validate(cfg, alpha, symbols);
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
