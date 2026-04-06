#pragma once

#include <set>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "util/util.hpp"

using Symbol = std::string;
using SymbolSet = std::set<Symbol>;
using Product = std::vector<SymbolSet>;
using ProductSet = std::set<Product>;

class Analyzer {
  public:
    explicit Analyzer(Config c, Alphabet a) : cfg_(std::move(c)), alpha_(std::move(a)) {
    }

    void set(const ProductSet& prodSet) {
        if (!(cfg_.L / cfg_.T == 4 && cfg_.P == 1))
            return;

        const auto prod = *prodSet.begin();
        const auto base = genSymbols(cfg_, alpha_);

        const auto prod1 = util::difference(base, prod[1]);
        const auto prod2 = util::difference(base, prod[2]);
        const auto prod3 = util::difference(base, prod[3]);
        const auto prodSet01 = expand({prod[0], prod[1]});
        const auto prodSet12 = expand({prod[1], prod2});
        const auto prodSet23 = expand({prod[2], prod3});
        const auto prodSet012 = expand({prod[0], prod[1], prod[2]});
        const auto prodSet123 = expand({prod[1], prod[2], prod3});

        result.clear();

        result.push_back(prod1.empty());
        result.push_back(prod[0] == prod1);
        result.push_back(!util::hasIntersection(prod[0], prod1));

        result.push_back(prod2.empty());
        result.push_back(prodSet01 == prodSet12);
        result.push_back(prod[0] == prod2 && !util::hasIntersection(prodSet01, prodSet12));
        result.push_back(!util::hasIntersection(prod[0], prod2) && !util::hasIntersection(prodSet01, prodSet12));

        result.push_back(prod3.empty());
        result.push_back(prodSet012 == prodSet123);
        result.push_back(prodSet01 == prodSet23 && !util::hasIntersection(prodSet012, prodSet123));
        result.push_back(prod[0] == prod3 && !util::hasIntersection(prodSet01, prodSet23) && !util::hasIntersection(prodSet012, prodSet123));
        result.push_back(!util::hasIntersection(prod[0], prod3) && !util::hasIntersection(prodSet01, prodSet23) && !util::hasIntersection(prodSet012, prodSet123));
    }

    std::vector<bool> getResult() const {
        return result;
    }

  private:
    SymbolSet genSymbols(const Config& cfg, const Alphabet& alpha) {
        SymbolSet symbols;

        const size_t limit = util::calcPower(alpha.size(), cfg.T);
        for (size_t i = 0; i < limit; ++i)
            symbols.insert(alpha.toSymbol(i, cfg.T));

        return symbols;
    }

    SymbolSet expand(const Product& p) {
        SymbolSet res{""};

        for (const auto& ss : p) {
            SymbolSet next;
            for (const auto& prefix : res)
                for (const auto& s : ss)
                    next.insert(prefix + s);
            res.swap(next);
        }

        return res;
    }

    Config cfg_;
    Alphabet alpha_;

    std::vector<bool> result;
};
