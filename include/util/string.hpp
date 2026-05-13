#pragma once

#include <sstream>
#include <string>
#include <string_view>

namespace util {

    template <class Container>
    std::string join(const Container& c, std::string_view delim, std::string pre = "", std::string suf = "") {
        std::ostringstream oss;
        for (auto it = c.begin(); it != c.end(); ++it) {
            if (it != c.begin())
                oss << delim;
            oss << *it;
        }
        return pre + oss.str() + suf;
    }

    std::vector<std::string> split(const std::string& str, char delim = '-') {
        std::vector<std::string> result;

        size_t start = 0;
        size_t pos = 0;

        while ((pos = str.find(delim, start)) != std::string::npos) {
            result.emplace_back(str, start, pos - start);
            start = pos + 1;
        }

        result.emplace_back(str, start);
        return result;
    }

    inline std::string to_lower(std::string_view str) {
        std::string res(str);

        std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        return res;
    }

} // namespace util
