#pragma once

#include "Analysis/Factory.hpp"
#include "Analysis/Strategy.hpp"

namespace Analysis {

    class Engine {
      public:
        Engine(const Config& cfg): strategy_(Factory::create(cfg)) {
        }

        void set(const ProductSet& prodSet) {
            result.clear();

            if (!strategy_)
                return;

            result = strategy_->run(prodSet);
        }

        std::vector<std::string> getResult() const {
            return result;
        }

      private:
        std::unique_ptr<Strategy> strategy_;
        std::vector<std::string> result;
    };

} // namespace Analysis
