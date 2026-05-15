#pragma once

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Types.hpp"

namespace Analysis {

    class Strategy {
      public:
        Strategy(const Config& cfg) : cfg(cfg) {}

        virtual ~Strategy() = default;

        std::vector<std::string> run(const ProductSet& products) {
            if (!validateInput(products))
                throw std::invalid_argument("Invalid input for the strategy.");

            std::vector<Product> sorted(products.begin(), products.end());
            std::sort(sorted.begin(), sorted.end());

            return buildRelations(sorted);
        }

      protected:
        Config cfg;

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

        std::string sizeChecker(const SymbolSet& s) const {
            return (s.size() == util::calcPower(cfg.Q, cfg.T)) ? "全" : "非全";
        }

        std::string hasIntersection(const Product& a, const Product& b) const {
            std::vector<std::string> res;
            for (size_t i = 0; i < a.size(); ++i)
                res.push_back(util::hasIntersection(a[i], b[i]) ? "T" : "F");
            return util::join(res, "");
        }

      private:
        virtual bool validateInput(const ProductSet&) const = 0;
        virtual std::vector<std::string> buildRelations(const std::vector<Product>&) const = 0;
    };

} // namespace Analysis
