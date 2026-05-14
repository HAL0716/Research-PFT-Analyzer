#pragma once

#include "Validator/Strategy.hpp"

namespace Validator {

    class L3_P2 : public Strategy {
      public:
        using Strategy::Strategy;

      private:
        bool checkPairRelation(const ProductSet& ps) const override {
            if (ps.size() != 2 || ps.begin()->size() != 3)
                return false;

            const auto& p1 = *ps.begin();
            const auto& p2 = *std::next(ps.begin());

            if (checkMergeable(p1, p2))
                return false;

            // 別のペアで定義できるため除外
            if (
                (p1[0] == p2[0] && util::hasIntersection(p1[1], p2[1]) && !util::hasIntersection(p1[2], p2[2])) ||
                (!util::hasIntersection(p1[0], p2[0]) && util::hasIntersection(p1[1], p2[1]) && p1[2] == p2[2]))
                return false;

            return true;
        };
    };

} // namespace Validator
