#pragma once

#include "Analysis/Strategy.hpp"

namespace Analysis {
    class L4_T1_P1 : public Strategy {
      public:
        std::vector<bool> analyze(const Config& cfg, const Alphabet& alpha, const ProductSet& prodSet) override {
            std::vector<bool> result;

            if (prodSet.empty())
                return result;

            const auto& prod = *prodSet.begin();
            const auto base = genSymbols(cfg, alpha);

            auto ctx = buildContext(prod, base);
            evaluate(prod, ctx, result);

            return result;
        }

      private:
        struct Context {
            SymbolSet p1c, p2c, p3c;
            SymbolSet s01, s12, s23;
            SymbolSet s012, s123;
        };

        Context buildContext(const Product& prod, const SymbolSet& base) {
            Context ctx;

            ctx.p1c = util::difference(base, prod[1]);
            ctx.p2c = util::difference(base, prod[2]);
            ctx.p3c = util::difference(base, prod[3]);

            ctx.s01 = expand({prod[0], prod[1]});
            ctx.s12 = expand({prod[1], ctx.p2c});
            ctx.s23 = expand({prod[2], ctx.p3c});
            ctx.s012 = expand({prod[0], prod[1], prod[2]});
            ctx.s123 = expand({prod[1], prod[2], ctx.p3c});

            return ctx;
        }

        void evaluate(const Product& prod, const Context& ctx, std::vector<bool>& result) {
            // --- prod1 ---
            add(result,
                ctx.p1c.empty(),
                prod[0] == ctx.p1c,
                disjoint(prod[0], ctx.p1c));

            // --- prod2 ---
            add(result,
                ctx.p2c.empty(),
                ctx.s01 == ctx.s12,
                prod[0] == ctx.p2c && disjoint(ctx.s01, ctx.s12),
                disjoint(prod[0], ctx.p2c) && disjoint(ctx.s01, ctx.s12));

            // --- prod3 ---
            add(result,
                ctx.p3c.empty(),
                ctx.s012 == ctx.s123,
                ctx.s01 == ctx.s23 && disjoint(ctx.s012, ctx.s123),
                prod[0] == ctx.p3c && disjoint(ctx.s01, ctx.s23) && disjoint(ctx.s012, ctx.s123),
                disjoint(prod[0], ctx.p3c) && disjoint(ctx.s01, ctx.s23) && disjoint(ctx.s012, ctx.s123));
        }

        template <typename... Args>
        void add(std::vector<bool>& result, bool v, Args... rest) {
            result.push_back(v);
            if constexpr (sizeof...(rest) > 0)
                add(result, rest...);
        }

        bool disjoint(const SymbolSet& a, const SymbolSet& b) const {
            return !util::hasIntersection(a, b);
        }

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
    };
} // namespace Analysis
