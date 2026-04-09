#include <iostream>
#include <set>
#include <vector>
#include <filesystem>

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
            return hasCorrectSize(ps) && hasFirstSymbol(ps) && hasNoIntersection(ps) && hasMappingInvariance(ps);
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
            std::vector<Product> v(ps.begin(), ps.end());

            auto intersect = [](const Product& a, const Product& b) {
                for (size_t i = 0; i < a.size(); ++i)
                    if (!util::hasIntersection(a[i], b[i]))
                        return false;
                return true;
            };

            for (size_t i = 0; i < v.size(); ++i)
                for (size_t j = i + 1; j < v.size(); ++j)
                    if (intersect(v[i], v[j]))
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
        const auto groups = util::Combinatorics::combs_r(perms, cfg.P);

        std::set<decltype(groups)::value_type> res;

        for (const auto& g : groups)
            if (calcSum(g) == cfg.N)
                res.insert(g);

        return res;
    }

    auto genProductSet(const Config& cfg, const SymbolSet& symbols, const Validator& isValid) {
        auto csv = util::createFile(cfg.toPath("step1"));

        const auto base = genBluePrint(cfg, symbols);
        size_t cnt = 0, total = base.size();
        for (const auto& group : base) {
            Logger::progress(++cnt, total, "Generating N = " + std::to_string(cfg.N) + ": ", true);

            std::vector<ProductSet> candidates;
            for (const auto& pattern : group) {
                std::vector<std::set<SymbolSet>> combs;
                for (auto n : pattern)
                    combs.push_back(util::Combinatorics::combs(symbols, n));

                candidates.push_back(util::Product::asVec(combs));
            }

            for (const auto& ps : util::Product::asSet(candidates))
                if (isValid(ps))
                    csv << util::join(Transform::toCsvRow(ps, cfg), ",") << "\n";
        }
    }

} // namespace

int main() {
    const bool UPDATE = false;

    const Config base("config.txt");

    const Alphabet alpha(base.Q);
    const SymbolSet symbols = genSymbols(base, alpha);

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = base.P; N <= maxN; ++N) {

        const auto cfg = base.withN(N);

        if (std::filesystem::exists(cfg.toPath("step1")) && !UPDATE)
            continue;

        const Validator validate(cfg, alpha, symbols);
        genProductSet(cfg, symbols, validate);
    }

    return 0;
}
