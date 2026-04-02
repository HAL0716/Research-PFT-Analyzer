#include <iostream>
#include <set>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

using Symbol = std::string;
using SymbolSet = std::set<Symbol>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

namespace {

    SymbolSet genSymbols(const Config& cfg, const Alphabet& alpha) {
        SymbolSet symbols;

        const size_t limit = util::calcPower(alpha.size(), cfg.T);
        for (size_t i = 0; i < limit; ++i)
            symbols.insert(alpha.toSymbol(i, cfg.T));

        return symbols;
    }

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
                mappedWords.insert(toWords(applyMap(ps, alpha_, m)));

            return toWords(ps) == *mappedWords.begin();
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
        const auto base = genBluePrint(cfg, symbols);

        std::set<ProductSet> result;

        size_t cnt = 0, total = base.size();
        for (const auto& group : base) {
            Logger::progress(++cnt, total, "Generating candidates: ", true);

            std::vector<ProductSet> candidates;

            for (const auto& pattern : group) {
                std::vector<std::set<SymbolSet>> combs;

                for (auto n : pattern)
                    combs.push_back(util::Combinatorics::combs(symbols, n));

                candidates.push_back(util::Product::asVec(combs));
            }

            for (const auto& ps : util::Product::asSet(candidates))
                if (isValid(ps))
                    result.insert(ps);
        }

        return result;
    }

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
    constexpr size_t Q = 2, T = 2, L = 4, P = 2;
    const size_t maxN = util::calcPower(Q, L);

    for (size_t N = 1; N <= maxN; ++N) {
        if (N < P)
            continue;
        Config cfg(Q, T, L, N, P);

        Alphabet alpha(cfg.Q);
        SymbolSet symbols = genSymbols(cfg, alpha);
        Validator validate(cfg, alpha, symbols);

        auto products = genProductSet(cfg, symbols, validate);

        if (!products.empty())
            writeCSV(products, cfg);
    }

    return 0;
}
