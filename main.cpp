#include <iostream>
#include <set>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "util/util.hpp"

using sizeVec = std::vector<size_t>;

using SymbolSet = std::set<std::string>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

bool isValid(const ProductSet& prodSet, const Config& cfg) {
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

    return true;
}

int main() {
    Config cfg(3, 2, 4, 4, 2);
    Alphabet alpha(cfg.Q);

    std::set<std::string> symbols;
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
            if (isValid(ps, cfg))
                res.insert(ps);
    }

    std::cout << "Total: " << res.size() << std::endl;

    return 0;
}
