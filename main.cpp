#include <iostream>
#include <set>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "util/util.hpp"

using sizeVec = std::vector<size_t>;

using Symbol = std::string;
using SymbolSet = std::set<Symbol>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

namespace {

    class Validator {
      public:
        Validator(const Config& cfg, const SymbolSet& symbols, const Alphabet& alpha)
            : cfg_(cfg), symbols_(symbols), alpha_(alpha) {
        }

        bool operator()(const ProductSet& prodSet) const {
            return checkFirstSymbol(prodSet) && checkSize(prodSet) && checkIntersection(prodSet) && checkMapping(prodSet);
        }

      private:
        const Config& cfg_;
        const SymbolSet& symbols_;
        const Alphabet& alpha_;

        bool checkFirstSymbol(const ProductSet& prodSet) const {
            const Symbol& target = *symbols_.begin();

            for (const auto& prod : prodSet)
                for (const auto& sym : prod[0])
                    if (sym == target)
                        return true;

            return false;
        }

        bool checkSize(const ProductSet& prodSet) const {
            return prodSet.size() == cfg_.P;
        }

        bool checkIntersection(const ProductSet& prodSet) const {
            auto hasIntersection = [](const Product& a, const Product& b) {
                for (size_t d = 0; d < a.size(); ++d)
                    if (!util::hasIntersection(a[d], b[d]))
                        return false;
                return true;
            };

            std::vector<Product> comb(prodSet.begin(), prodSet.end());
            for (size_t i = 0; i < comb.size(); ++i)
                for (size_t j = i + 1; j < comb.size(); ++j)
                    if (hasIntersection(comb[i], comb[j]))
                        return false;

            return true;
        }

        ProductSet applyMap(const ProductSet& prodSet, const Symbol& map) const {
            ProductSet mapSet;

            for (const auto& prod : prodSet) {
                Product mapProd;

                for (const auto& symSet : prod) {
                    SymbolSet mapSymSet;
                    for (const auto& sym : symSet)
                        mapSymSet.insert(alpha_.add(sym, map));

                    mapProd.push_back(mapSymSet);
                }

                mapSet.insert(mapProd);
            }

            return mapSet;
        }

        bool checkMapping(const ProductSet& prodSet) const {

            std::set<SymbolSet> mappedWords;
            for (const auto& map : symbols_)
                mappedWords.insert(toWords(applyMap(prodSet, map)));

            return toWords(prodSet) == *mappedWords.begin();
        }

        SymbolSet toWords(const ProductSet& ps) const {
            auto expand = [](const Product& p) -> SymbolSet {
                SymbolSet res{""};
                for (const SymbolSet& ss : p) {
                    SymbolSet next;
                    for (const Symbol& prefix : res)
                        for (const Symbol& s : ss)
                            next.insert(prefix + s);
                    res.swap(next);
                }
                return res;
            };

            SymbolSet words;
            for (const Product& p : ps) {
                SymbolSet tmp = expand(p);
                words.insert(tmp.begin(), tmp.end());
            }
            return words;
        }
    };

} // namespace

int main() {
    Config cfg(3, 2, 4, 4, 2);
    Alphabet alpha(cfg.Q);

    SymbolSet symbols;
    for (size_t i = 0; i < util::calcPower(alpha.size(), cfg.T); ++i)
        symbols.insert(alpha.toSymbol(i, cfg.T));

    Validator isValid(cfg, symbols, alpha);

    const auto indices = util::range(1, symbols.size());
    const auto perm_indices = util::Combinatorics::perms_r(indices, cfg.L / cfg.T);
    const auto comb_groups = util::Combinatorics::combs_r(perm_indices, cfg.P);

    std::set<decltype(comb_groups)::value_type> filtered_groups;
    for (const auto& group : comb_groups) {
        util::ull sum = 0;
        for (const auto& pattern : group)
            sum += util::calcProduct(pattern);

        if (sum == cfg.N)
            filtered_groups.insert(group);
    }

    std::set<ProductSet> res;
    for (const auto& group : filtered_groups) {
        std::vector<ProductSet> prodVec;
        for (const auto& pattern : group) {
            std::vector<std::set<SymbolSet>> combsSet;
            for (size_t num : pattern)
                combsSet.push_back(util::Combinatorics::combs(symbols, num));
            prodVec.push_back(util::Product::asVec(combsSet));
        }
        for (const auto& ps : util::Product::asSet(prodVec))
            if (isValid(ps))
                res.insert(ps);
    }

    auto csv = util::createFile(cfg.toPath());
    for (const auto& ps : res) {
        std::set<std::string> psStr;
        for (const auto& prod : ps) {
            std::vector<std::string> prodStr;
            for (const auto& symSet : prod)
                prodStr.push_back(util::join(symSet, "-"));
            psStr.insert(util::join(prodStr, ","));
        }
        csv << util::join(psStr, ",") << std::endl;
    }
    std::cout << cfg.toPath() << " Saved." << std::endl;

    return 0;
}
