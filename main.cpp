#include <iostream>
#include <set>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "util/util.hpp"

using sizeVec = std::vector<size_t>;

using SymbolSet = std::set<std::string>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

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
        std::set<ProductSet> prodSet;
        for (const auto& pattern : group) {
            std::vector<std::set<SymbolSet>> combsSet;
            for (size_t num : pattern)
                combsSet.push_back(util::Combinatorics::combs(symbols, num));
            prodSet.insert(util::Product::asVec(combsSet));
        }
        auto expanded_products = util::Product::asSet(prodSet);
        res.insert(expanded_products.begin(), expanded_products.end());
    }

    std::cout << "Total: " << res.size() << std::endl;

    return 0;
}
