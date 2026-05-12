#pragma once

#include "Analysis/Strategy.hpp"

namespace Analysis {

    class L4_P1 : public Strategy {
      public:
        using Strategy::Strategy;

      private:
        bool validateInput(const ProductSet& ps) const override {
            return ps.size() == 1 && ps.begin()->size() == 4;
        }

        std::vector<std::string> buildRelations(const std::vector<Product>& p) const override {
            const auto& a = p[0];

            return {
                util::join(
                    std::vector{
                        classifyRelation(a[0], a[1], "0", "1"),
                        classifyRelation(a[0], a[2], "0", "2"),
                        // classifyRelation(a[0], a[3], "0", "3"),
                        classifyRelation(a[1], a[2], "1", "2"),
                        classifyRelation(a[1], a[3], "1", "3"),
                        classifyRelation(a[2], a[3], "2", "3"),
                    },
                    "-")};
        };
    };

} // namespace Analysis
