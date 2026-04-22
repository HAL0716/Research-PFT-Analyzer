#pragma once

#include "Analysis/Strategy.hpp"
#include "util/util.hpp"

namespace Analysis {

    class L3_P1 : public Strategy {
      public:
        std::vector<std::string> analyze(const Config&, const Alphabet&, const ProductSet& prodSet) override {
            if (!isValid(prodSet))
                throw std::invalid_argument("Invalid product set");

            const auto& p = *prodSet.begin();

            return {
                evaluate(p[0], p[1], "0", "1"),
                // evaluate(p[0], p[2], "0", "2"),
                evaluate(p[1], p[2], "1", "2")};
        }

      private:
        bool isValid(const ProductSet& ps) const {
            return ps.size() == 1 && ps.begin()->size() == 3;
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
