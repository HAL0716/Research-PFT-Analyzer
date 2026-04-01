#include <iostream>

#include "Alphabet.hpp"
#include "util.hpp"

int main() {
    size_t Q = 3, T = 2;
    Alphabet alpha(Q);

    for (size_t i = 0; i < util::calcPower(alpha.size(), T); ++i)
        std::cout << alpha.toSymbol(i, T) << std::endl;

    return 0;
}
