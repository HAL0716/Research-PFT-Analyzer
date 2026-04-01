#include <iostream>
#include <set>

#include "Alphabet.hpp"
#include "util.hpp"

int main() {
    size_t Q = 3, T = 2, L = T * 2, N = 6, P = 2;
    Alphabet alpha(Q);

    std::set<std::string> symbols;
    for (size_t i = 0; i < util::calcPower(alpha.size(), T); ++i)
        symbols.insert(alpha.toSymbol(i, T));

    auto perms = util::perms_r(util::range(1, symbols.size()), L / T);
    auto combs = util::combs_r(perms, P);

    for (const auto& c : combs) {
        util::ull sum = 0;
        for (const auto& p : c)
            sum += util::calcProduct(p);

        if (sum == N) {
            std::vector<std::string> s;
            for (const auto& p : c)
                s.push_back(util::join(p, ",", "{", "}"));
            std::cout << util::join(s, "+") << std::endl;
        }
    }

    return 0;
}
