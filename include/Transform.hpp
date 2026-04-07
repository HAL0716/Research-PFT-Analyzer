#pragma once

#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Config.hpp"
#include "Types.hpp"

namespace Transform {

    auto expand(const Product& p) {
        SymbolSet res;
        res.insert("");
        for (const auto& ss : p) {
            SymbolSet next;
            for (const auto& prefix : res) {
                for (const auto& s : ss) {
                    std::string tmp;
                    tmp.reserve(prefix.size() + s.size());
                    tmp += prefix;
                    tmp += s;
                    next.insert(std::move(tmp));
                }
            }
            res.swap(next);
        }
        return res;
    }

    auto toWords(const ProductSet& ps) {
        SymbolSet res;
        for (const auto& p : ps) {
            auto tmp = expand(p);
            for (auto& v : tmp)
                res.insert(std::move(v));
        }
        return res;
    }

    auto toProductSet(const util::csvRow& row, const Config& cfg) {
        if (row.size() != cfg.L / cfg.T)
            throw std::invalid_argument("invalid row size");

        ProductSet res;
        for (size_t i = 0; i < row.size(); i += cfg.L / cfg.T) {
            Product p;
            for (size_t j = 0; j < cfg.L / cfg.T; ++j) {
                SymbolSet ss;
                std::stringstream ssStream(row[i + j]);
                std::string sym;

                while (std::getline(ssStream, sym, '-'))
                    ss.insert(sym);

                p.push_back(std::move(ss));
            }
            res.insert(std::move(p));
        }
        return res;
    }

} // namespace Transform
