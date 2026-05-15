#pragma once

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Transform.hpp"
#include "Types.hpp"

namespace Validator {

    class Strategy {
      public:
        Strategy(const Config& cfg, const Alphabet& alpha, const SymbolSet& symbols) : cfg(cfg), alpha(alpha), symbols(symbols) {
        }

        virtual ~Strategy() = default;

        bool operator()(const ProductSet& ps) const {
            return checkSize(ps) && checkFirstSymbol(ps) && checkPairRelation(ps) && checkMapping(ps);
        }

      protected:
        const Config& cfg;
        const Alphabet& alpha;
        const SymbolSet& symbols;

        bool checkMergeable(const Product& a, const Product& b) const {
            size_t diff = 0;
            for (size_t i = 0; i < a.size(); ++i)
                if (a[i] != b[i])
                    ++diff;
            return diff == 1;
        }

      private:
        virtual bool checkSize(const ProductSet& ps) const {
            return ps.size() == cfg.P && Transform::toWords(ps).size() == cfg.N;
        }

        virtual bool checkFirstSymbol(const ProductSet& ps) const {
            const Symbol& target = *symbols.begin();
            for (const auto& p : ps)
                for (const auto& s : p[0])
                    if (s == target)
                        return true;
            return false;
        }

        virtual bool checkPairRelation(const ProductSet&) const {
            return true;
        }

        virtual bool checkMapping(const ProductSet& ps) const {
            auto applyMap = [&](const ProductSet& ps, const Symbol& map) {
                auto genMapped = [&](const Product& p) {
                    Product res;
                    for (const auto& symSet : p) {
                        SymbolSet mappedSet;
                        for (const auto& sym : symSet)
                            mappedSet.insert(alpha.add(sym, map));
                        res.push_back(mappedSet);
                    }
                    return res;
                };

                ProductSet res;
                for (const auto& p : ps)
                    res.insert(genMapped(p));
                return res;
            };

            std::set<SymbolSet> mappedWords;
            for (const auto& m : symbols)
                mappedWords.insert(Transform::toWords(applyMap(ps, m)));
            return Transform::toWords(ps) == *mappedWords.begin();
        }
    };

} // namespace Validator
