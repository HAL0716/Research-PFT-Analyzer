#pragma once

#include "Analysis/Strategy.hpp"
#include "util/util.hpp"

namespace Analysis {

    class L4_T1_P1 : public Strategy {
      public:
        std::vector<std::string> analyze(const Config& cfg, const Alphabet&, const ProductSet& prodSet) override {
            if (!isValid(prodSet))
                throw std::invalid_argument("Invalid product set");

            const auto& p = *prodSet.begin();

            return {
                evaluate(p[0], p[1], "S0", "S1"),
                evaluate(p[0], p[2], "S0", "S2"),
                p[0].size() == util::calcPower(cfg.Q, cfg.T) ? "全" : "非全",
                evaluate(p[1], p[2], "S1", "S2"),
                evaluate(p[1], p[3], "S1", "S3"),
                evaluate(p[2], p[3], "S2", "S3")};
        }

      private:
        bool isValid(const ProductSet& ps) const {
            return ps.size() == 1 && ps.begin()->size() == 4;
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
