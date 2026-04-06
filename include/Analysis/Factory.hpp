#pragma once

#include <memory>

#include "Analysis/Strategy.hpp"
#include "Analysis/L4_T1_P1.hpp"

namespace Analysis {
    class Factory {
      public:
        static std::unique_ptr<Strategy> create(const Config& cfg) {
            if (cfg.L / cfg.T == 4 && cfg.P == 1)
                return std::make_unique<L4_T1_P1>();

            return nullptr;
        }
    };
} // namespace Analysis
