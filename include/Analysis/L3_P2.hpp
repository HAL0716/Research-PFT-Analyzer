#pragma once

#include <stdexcept>

#include "Analysis/Strategy.hpp"
#include "util/util.hpp"

namespace Analysis {

    class L3_P2 : public Strategy {
      private:
        bool validateInput(const ProductSet& ps) const override {
            return ps.size() == 2 && ps.begin()->size() == 3;
        }

        std::vector<std::string> compute(const ProductSet& prodSet) override {
            const std::vector<Product>& products = std::vector<Product>(prodSet.begin(), prodSet.end());
            const auto& p0 = products[0];
            const auto& p1 = products[1];

            return {
                classifyRelation(p0[0], p0[1], "0", "1"),
                // classifyRelation(p0[0], p0[2], "0", "2"),
                classifyRelation(p0[1], p0[2], "1", "2"),
                classifyRelation(p1[0], p1[1], "0", "1"),
                // classifyRelation(p1[0], p1[2], "0", "2"),
                classifyRelation(p1[1], p1[2], "1", "2"),
                classifyRelation(p0[0], p1[0], "0", "1"),
                classifyRelation(p0[0], p1[1], "0", "1"),
                classifyRelation(p0[0], p1[2], "0", "1"),
                classifyRelation(p0[1], p1[0], "0", "1"),
                classifyRelation(p0[1], p1[1], "0", "1"),
                classifyRelation(p0[1], p1[2], "0", "1"),
                classifyRelation(p0[2], p1[0], "0", "1"),
                classifyRelation(p0[2], p1[1], "0", "1"),
                classifyRelation(p0[2], p1[2], "0", "1"),
                std::to_string(p0[0].size()),
                std::to_string(p0[1].size()),
                std::to_string(p0[2].size()),
                std::to_string(p1[0].size()),
                std::to_string(p1[1].size()),
                std::to_string(p1[2].size()),
            };
        }
    };

} // namespace Analysis
