#pragma once

#include "Analysis/Strategy.hpp"

namespace Analysis {

    class L3_P1 : public Strategy {
      private:
        bool validateInput(const ProductSet& ps) const override {
            return ps.size() == 1 && ps.begin()->size() == 3;
        }

        std::vector<std::string> buildRelations(const std::vector<Product>& p) const override {
            const auto& a = p[0];

            return {
                util::join(
                    std::vector{
                        classifyRelation(a[0], a[1], "0", "1"),
                        // classifyRelation(a[0], a[2], "0", "2"),
                        classifyRelation(a[1], a[2], "1", "2"),
                    },
                    "-")};
        }
    };

} // namespace Analysis
