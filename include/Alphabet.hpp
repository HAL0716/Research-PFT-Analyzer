#pragma once

#include <array>
#include <stdexcept>
#include <string>

class Alphabet {
  public:
    Alphabet() = delete;

    explicit Alphabet(size_t Q) {
        if (Q > BASE.size())
            throw std::invalid_argument("Q must be at most 36");
        used = BASE.substr(0, Q);

        map.fill(-1);
        for (size_t i = 0; i < used.size(); ++i)
            map[(unsigned char)used[i]] = (int)i;
    }

    size_t size() const {
        return used.size();
    }

    int toIdx(char c) const {
        int idx = map[(unsigned char)c];
        if (idx == -1)
            throw std::invalid_argument("invalid character");
        return idx;
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

  private:
    static constexpr std::string_view BASE = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::string used;
    std::array<int, 256> map{};

    char toChar(size_t i) const {
        if (i >= used.size())
            throw std::out_of_range("out of range");
        return used[i];
    }
};
