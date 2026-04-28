#pragma once

#include <stdexcept>

#include "Analysis/Strategy.hpp"
#include "util/util.hpp"

namespace Analysis {

    class L3_P2 : public Strategy {
      public:
        std::vector<std::string> analyze(const Config&, const Alphabet&, const ProductSet& prodSet) override {
            if (!isValid(prodSet))
                throw std::invalid_argument("Invalid product set");

            const std::vector<Product>& products = std::vector<Product>(prodSet.begin(), prodSet.end());
            const auto& p0 = products[0];
            const auto& p1 = products[1];

            return {
                evaluate(p0[0], p0[1], "0", "1"),
                // evaluate(p0[0], p0[2], "0", "2"),
                evaluate(p0[1], p0[2], "1", "2"),
                evaluate(p1[0], p1[1], "0", "1"),
                // evaluate(p1[0], p1[2], "0", "2"),
                evaluate(p1[1], p1[2], "1", "2"),
                evaluate(p0[0], p1[0], "0", "1"),
                evaluate(p0[1], p1[1], "0", "1"),
                evaluate(p0[2], p1[2], "0", "1"),
            };
        }

      private:
        bool isValid(const ProductSet& ps) const {
            return ps.size() == 2 && ps.begin()->size() == 3;
        }

        std::string evaluate(const SymbolSet& a, const SymbolSet& b, const std::string& labelA, const std::string& labelB) const {
            if (!util::hasIntersection(a, b))
                return "素";

            if (a == b)
                return "等";

            if (util::isSubset(a, b))
                return labelB;

            if (util::isSubset(b, a))
                return labelA;

            return "交";
        }
    };

} // namespace Analysis
