#pragma once

#include <set>
#include <string>

#include "Alphabet.hpp"
#include "Config.hpp"

class Analyzer {
  public:
    explicit Analyzer(Config c, Alphabet a) : cfg_(std::move(c)), alpha_(std::move(a)) {
    }

    auto run(const std::set<std::string>& w) const {
        // TODO: 実装中
    }

  private:
    Config cfg_;
    Alphabet alpha_;
};
