#include <iostream>

#include "util.hpp"

int main() {
    util::ull base = 2;
    util::ull exp = 10;
    util::ull result = util::calcPower(base, exp);
    std::cout << base << "^" << exp << " = " << result << std::endl;
    return 0;
}
