#pragma once

#include <stdexcept>

#include "Analysis/Strategy.hpp"
#include "util/util.hpp"

namespace Analysis {

    class L4_P1 : public Strategy {
      private:
        bool validateInput(const ProductSet& ps) const override {
            return ps.size() == 1 && ps.begin()->size() == 4;
        }

        std::vector<std::string> compute(const ProductSet& prodSet) override {
            const std::vector<Product>& products = std::vector<Product>(prodSet.begin(), prodSet.end());
            const auto& p0 = products[0];

            return {
                classifyRelation(p0[0], p0[1], "0", "1"),
                classifyRelation(p0[0], p0[2], "0", "2"),
                // classifyRelation(p0[0], p0[3], "0", "3"),
                classifyRelation(p0[1], p0[2], "1", "2"),
                classifyRelation(p0[1], p0[3], "1", "3"),
                classifyRelation(p0[2], p0[3], "2", "3"),
            };
        }
    };

} // namespace Analysis
