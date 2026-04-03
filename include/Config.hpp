#pragma once

#include <cstddef>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>

#include "util/util.hpp"

struct Config {
    size_t Q = 2, T = 2, L = 4, N = 4, P = 2; // default values
    std::string outDir;

    explicit Config(const std::string& inFile, const std::string& outDir = "output")
        : outDir(outDir) {
        set(inFile);
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
    void set(const std::string& path) {
        auto data = util::readCSV(path);

        for (const auto& row : data) {
            if (row.size() != 2)
                continue;

            const auto& key = row[0];
            const auto& val = row[1];

            if (key == "Q")
                Q = std::stoul(val);
            else if (key == "T")
                T = std::stoul(val);
            else if (key == "L")
                L = std::stoul(val);
            else if (key == "N")
                N = std::stoul(val);
            else if (key == "P")
                P = std::stoul(val);
        }
    }

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
