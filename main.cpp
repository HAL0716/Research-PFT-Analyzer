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

std::set<std::string> toWords(const ProductSet& ps) {
    auto expand = [](const Product& p) -> std::set<std::string> {
        std::set<std::string> res{""};
        for (const SymbolSet& ss : p) {
            std::set<std::string> next;
            for (const Symbol& prefix : res)
                for (const Symbol& s : ss)
                    next.insert(prefix + s);
            res.swap(next);
        }
        return res;
    };

    std::set<std::string> words;
    for (const Product& p : ps) {
        std::set<std::string> tmp = expand(p);
        words.insert(tmp.begin(), tmp.end());
    }

    return words;
}

bool isValid(const ProductSet& prodSet, const Config& cfg, const SymbolSet& symbols, const Alphabet& alpha) {
    SymbolSet firstSymbols;
    for (const auto& prod : prodSet)
        firstSymbols.insert(prod[0].begin(), prod[0].end());
    if (!firstSymbols.contains(*symbols.begin()))
        return false;

    auto hasIntersection = [&](const Product& a, const Product& b) -> bool {
        for (size_t d = 0; d < a.size(); ++d)
            if (!util::hasIntersection(a[d], b[d]))
                return false;

        return true;
    };

    std::vector<Product> comb(prodSet.begin(), prodSet.end());

    const size_t n = comb.size();
    if (n != cfg.P)
        return false;

    for (size_t i = 0; i < n; ++i)
        for (size_t j = i + 1; j < n; ++j)
            if (hasIntersection(comb[i], comb[j]))
                return false;

    std::set<ProductSet> mapSets;
    for (const auto& map : symbols) {
        ProductSet mapSet;
        for (const auto& prod : prodSet) {
            Product mapProd;
            for (const auto& symSet : prod) {
                SymbolSet mapSymSet;
                for (const auto& sym : symSet) {
                    mapSymSet.insert(alpha.add(sym, map));
                }
                mapProd.push_back(mapSymSet);
            }
            mapSet.insert(mapProd);
        }
        mapSets.insert(mapSet);
    }

    const auto words = toWords(prodSet);
    std::set<std::set<std::string>> mapWords;
    for (const auto& mapSet : mapSets)
        mapWords.insert(toWords(mapSet));

    return words == *mapWords.begin();
}

int main() {
    Config cfg(3, 2, 4, 4, 2);
    Alphabet alpha(cfg.Q);

    SymbolSet symbols;
    for (size_t i = 0; i < util::calcPower(alpha.size(), cfg.T); ++i)
        symbols.insert(alpha.toSymbol(i, cfg.T));

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
            if (isValid(ps, cfg, symbols, alpha))
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
