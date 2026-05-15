#pragma once

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Transform.hpp"
#include "Types.hpp"
#include "Validator/Factory.hpp"
#include "Validator/Strategy.hpp"

namespace Validator {

    class Engine {
      public:
        Engine(const Config& cfg, const Alphabet& alpha, const SymbolSet& symbols) : strategy_(Factory::create(cfg, alpha, symbols)) {
        }

        bool operator()(const ProductSet& ps) const {
            if (!strategy_)
                return false;

            return (*strategy_)(ps);
        }

      private:
        std::unique_ptr<Strategy> strategy_;
    };

} // namespace Validator
