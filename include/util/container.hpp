#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

namespace util {

    template <typename Container>
    void normalize(Container& c) {
        std::sort(c.begin(), c.end());
        c.erase(std::unique(c.begin(), c.end()), c.end());
    }

    inline std::vector<size_t> range(size_t st, size_t ed) {
        std::vector<size_t> v(ed - st);
        std::iota(v.begin(), v.end(), st);
        return v;
    }

} // namespace util
