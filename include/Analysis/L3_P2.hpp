#pragma once

#include "Analysis/Strategy.hpp"

namespace Analysis {

    class L3_P2 : public Strategy {
      public:
        using Strategy::Strategy;

      private:
        bool validateInput(const ProductSet& ps) const override {
            return ps.size() == 2 && ps.begin()->size() == 3;
        }

        std::vector<std::string> buildRelations(const std::vector<Product>& p) const override {
            auto build = [&](const Product& a, const Product& b) {
                const Product c{
                    util::setUnion(a[0], b[0]),
                    util::setUnion(a[1], b[1]),
                    util::setUnion(a[2], b[2]),
                };

                return util::join(
                    std::vector{
                        // 1-1-2-1
                        classifyRelation(a[0], a[1], "A0", "A1"),
                        classifyRelation(a[1], a[2], "A1", "A2"),
                        classifyRelation(b[0], b[1], "B0", "B1"),
                        classifyRelation(b[1], b[2], "B1", "B2"),
                        classifyRelation(a[0], b[0], "A0", "B0"),

                        // 1-2-1-1
                        classifyRelation(a[1], b[2], "A1", "B2"),
                        classifyRelation(a[2], b[1], "A2", "B1"),
                        classifyRelation(a[0], b[0], "A0", "B0"),
                        classifyRelation(a[0], c[1], "A0", "C1"),
                        classifyRelation(b[0], c[1], "B0", "C1"),
                        classifyRelation(a[1], c[2], "A1", "C2"),
                        classifyRelation(b[1], c[2], "B1", "C2"),
                        classifyRelation(a[2], b[2], "A2", "B2"),
                    },
                    "-");
            };

            std::vector<std::string> res = {
                build(p[0], p[1]),
                build(p[1], p[0]),
            };
            util::normalize(res);

            return res;
        }
    };

} // namespace Analysis
