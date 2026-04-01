#include <iostream>
#include <set>

#include "Alphabet.hpp"
#include "util.hpp"

using sizeVec = std::vector<size_t>;

using SymbolSet = std::set<std::string>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

int main() {
    size_t Q = 3, T = 2, L = T * 2, N = 4, P = 2;
    Alphabet alpha(Q);

    std::set<std::string> symbols;
    for (size_t i = 0; i < util::calcPower(alpha.size(), T); ++i)
        symbols.insert(alpha.toSymbol(i, T));

    const auto idxs = util::range(1, symbols.size());
    const auto perms = util::perms_r(idxs, L / T);
    const auto combs = util::combs_r(perms, P);

    std::set<decltype(combs)::value_type> combs2;
    for (const auto& c : combs) {
        util::ull sum = 0;
        for (const auto& p : c)
            sum += util::calcProduct(p);

        if (sum == N)
            combs2.insert(c);
    }

    std::set<ProductSet> result;
    for (const auto& c : combs2) {
        std::set<ProductSet> prodSet;
        for (const auto& p : c) {
            std::vector<std::set<SymbolSet>> s;
            for (size_t i : p)
                s.push_back(util::combs(symbols, i));
            auto prod = util::prod_V(s);
            prodSet.insert(prod);
        }
        auto prod2 = util::prod_S(prodSet);
        result.insert(prod2.begin(), prod2.end());
    }

    std::cout << "Total: " << result.size() << std::endl;

    return 0;
}
