#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

class Alphabet {
  public:
    Alphabet() = delete;

    explicit Alphabet(size_t Q) {
        if (Q > BASE.size())
            throw std::invalid_argument("Q must be at most 36");
        used = BASE.substr(0, Q);

        map.fill(SIZE_MAX);
        for (size_t i = 0; i < used.size(); ++i)
            map[(unsigned char)used[i]] = i;
    }

    size_t size() const {
        return used.size();
    }

    size_t toValue(const std::string& s) const {
        size_t value = 0;

        for (char c : s) {
            size_t digit = toIdx(c);
            if (value > (SIZE_MAX - digit) / size())
                throw std::overflow_error("value overflow");
            value = value * size() + digit;
        }

        return value;
    }

    std::string toSymbol(size_t value, size_t length = 1) const {
        std::string result;

        if (value == 0) {
            result = std::string(1, toChar(0));
        } else {
            while (value > 0) {
                result += toChar(value % size());
                value /= size();
            }
            std::reverse(result.begin(), result.end());
        }

        if (result.size() < length)
            result = std::string(length - result.size(), toChar(0)) + result;

        return result;
    }

    std::string add(const std::string& a, const std::string& b) const {
        std::string result(a.size(), toChar(0));
        for (size_t i = 0; i < a.size(); ++i) {
            size_t digitA = toIdx(a[i]);
            size_t digitB = toIdx(b[i]);
            result[i] = toChar((digitA + digitB) % size());
        }
        return result;
    }

  private:
    static constexpr std::string_view BASE = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::string used;
    std::array<size_t, 256> map{};

    size_t toIdx(char c) const {
        size_t idx = map[(unsigned char)c];
        if (idx == SIZE_MAX)
            throw std::invalid_argument("invalid character");
        return idx;
    }

    char toChar(size_t i) const {
        if (i >= used.size())
            throw std::out_of_range("out of range");
        return used[i];
    }
};
