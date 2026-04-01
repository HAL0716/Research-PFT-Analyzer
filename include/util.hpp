#pragma once

namespace util {
    using ull = unsigned long long;

    ull calcPower(ull base, ull exp) {
        ull result = 1;
        for (ull i = 0; i < exp; ++i)
            result *= base;
        return result;
    }
} // namespace util
