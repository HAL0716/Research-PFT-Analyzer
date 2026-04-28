#pragma once

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Types.hpp"

namespace Analysis {

    class Strategy {
      public:
        virtual ~Strategy() = default;
        std::vector<std::string> run(const ProductSet& products) {
            if (!validateInput(products))
                throw std::invalid_argument("Invalid input for the strategy.");
            return compute(products);
        }

      protected:
        std::string classifyRelation(const SymbolSet& a, const SymbolSet& b, const std::string& labelA, const std::string& labelB) const {
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

      private:
        virtual bool validateInput(const ProductSet&) const = 0;
        virtual std::vector<std::string> compute(const ProductSet&) = 0;
    };

} // namespace Analysis
