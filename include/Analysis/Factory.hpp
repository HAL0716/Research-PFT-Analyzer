#pragma once

#include <memory>

#include "Analysis/L3_P1.hpp"
#include "Analysis/L3_P2.hpp"
#include "Analysis/L4_P1.hpp"
#include "Analysis/Strategy.hpp"

namespace Analysis {

    class Factory {
      public:
        static std::unique_ptr<Strategy> create(const Config& cfg) {
            if (cfg.L / cfg.T == 3 && cfg.P == 1)
                return std::make_unique<L3_P1>(cfg);
            if (cfg.L / cfg.T == 3 && cfg.P == 2)
                return std::make_unique<L3_P2>(cfg);
            if (cfg.L / cfg.T == 4 && cfg.P == 1)
                return std::make_unique<L4_P1>(cfg);

            return nullptr;
        }
    };

} // namespace Analysis
