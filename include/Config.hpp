#pragma once

#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>

#include "util/util.hpp"

struct Config {
    size_t Q = 2;
    size_t T = 2;
    size_t L = 4;
    size_t N = 4;
    size_t P = 2;

    std::string outDir = "output";

    Config() {
        validate();
    }

    Config(std::string outDir) : outDir(std::move(outDir)) {
        validate();
    }

    Config withN(size_t newN) const {
        Config c = *this;
        c.N = newN;
        c.validate();
        return c;
    }

    std::string toPath() const {
        return std::format("{}/T={}_L={}_P={}_Q={}/N={}.csv", outDir, T, L, P, Q, N);
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
