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

} // namespace util
