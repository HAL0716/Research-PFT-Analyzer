#pragma once

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Types.hpp"

namespace Analysis {
    class Strategy {
      public:
        virtual ~Strategy() = default;

        virtual std::vector<bool> analyze(const Config& cfg, const Alphabet& alpha, const ProductSet& prodSet) = 0;
    };
} // namespace Analysis
