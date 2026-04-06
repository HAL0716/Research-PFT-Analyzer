#pragma once

#include "Analysis/Factory.hpp"
#include "Analysis/Strategy.hpp"

namespace Analysis {
    class Engine {
      public:
        Engine(Config c, Alphabet a)
            : cfg_(std::move(c)), alpha_(std::move(a)), strategy_(Factory::create(cfg_)) {
        }

        void set(const ProductSet& prodSet) {
            result.clear();

            if (!strategy_)
                return;

            result = strategy_->analyze(cfg_, alpha_, prodSet);
        }

        std::vector<bool> getResult() const {
            return result;
        }

      private:
        Config cfg_;
        Alphabet alpha_;
        std::unique_ptr<Strategy> strategy_;
        std::vector<bool> result;
    };
} // namespace Analysis
