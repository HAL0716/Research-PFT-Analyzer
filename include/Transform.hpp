#pragma once

#include <set>
#include <string>
#include <utility>

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

} // namespace Transform
