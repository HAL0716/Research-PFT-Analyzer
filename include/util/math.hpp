#pragma once

namespace util {

    using ull = unsigned long long;

    inline ull calcPower(ull base, ull exp) {
        ull res = 1;
        while (exp--)
            res *= base;
        return res;
    }

    template <typename Container>
    ull calcProduct(const Container& v) {
        ull res = 1;
        for (const auto& x : v)
            res *= x;
        return res;
    }

    template <typename NumType>
    inline ull popcount(NumType x) {
        ull count = 0;
        while (x) {
            x &= (x - 1);
            ++count;
        }
        return count;
    }

} // namespace util
