#pragma once

#include <memory>

#include "Validator/L3_P2.hpp"
#include "Validator/Strategy.hpp"

namespace Validator {

    class Factory {
      public:
        static std::unique_ptr<Strategy> create(const Config& cfg, const Alphabet& alpha, const SymbolSet& symbols) {
            if (cfg.L / cfg.T == 3 && cfg.P == 2)
                return std::make_unique<L3_P2>(cfg, alpha, symbols);

            return std::make_unique<Strategy>(cfg, alpha, symbols);
        }
    };

} // namespace Validator
