#pragma once

#include <cstddef>
#include <stdexcept>

#include "util/util.hpp"

struct Config {
    size_t Q, T, L, N, P;

    constexpr Config(size_t q, size_t t, size_t l, size_t n, size_t p) noexcept
        : Q(q), T(t), L(l), N(n), P(p) {
        validate();
    }

  private:
    void validate() const {
        if (Q == 0)
            throw std::invalid_argument("Q");
        if (T == 0)
            throw std::invalid_argument("T");
        if (L == 0 || L % T != 0)
            throw std::invalid_argument("L");
        if (N == 0 || N > util::calcPower(Q, L))
            throw std::invalid_argument("N");
        if (P == 0 || P > N || P > util::calcPower(Q, T))
            throw std::invalid_argument("P");
    }
};
