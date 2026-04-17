#pragma once

#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "util/util.hpp"

struct Config {
    size_t Q = 2, T = 2, L = 4, N = 4, P = 2, V = 3;
    size_t featureNum = 0, targetNum = 0;

    explicit Config(const std::string& configFile = "config.txt") {
        init(configFile);
    }

    Config withN(size_t newN) const {
        Config c = *this;
        c.N = newN;
        c.validate();
        return c;
    }

    std::string toPath(const std::string& dir = "", bool isIndividual = true, const std::string& ext = ".csv") const {
        std::string base = RESULT_DIR;
        if (!dir.empty())
            base += "/" + dir;
        if (isIndividual)
            return std::format("{}/T={}_L={}_P={}_Q={}/N={}{}", base, T, L, P, Q, N, ext);
        else
            return std::format("{}/T={}_L={}_P={}_Q={}{}", base, T, L, P, Q, ext);
    }

  private:
    static constexpr const char* RESULT_DIR = "output";

    void init(const std::string& configFile) {
        set(configFile);
        validate();
    }

    void set(const std::string& path) {
        auto data = util::readCSV(path, util::FileErrorPolicy::THROW);

        const std::unordered_map<std::string, size_t*> table = {
            {"Q", &Q}, {"T", &T}, {"L", &L}, {"N", &N}, {"P", &P}, {"V", &V}, {"FEATURE_NUM", &featureNum}, {"TARGET_NUM", &targetNum}};

        for (const auto& row : data) {
            if (row.size() != 2)
                continue;

            auto it = table.find(row[0]);
            if (it != table.end()) {
                *(it->second) = std::stoul(row[1]);
            }
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
        if (V == 0)
            throw std::invalid_argument("V");
    }
};
