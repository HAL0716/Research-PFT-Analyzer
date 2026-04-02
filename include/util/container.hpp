#pragma once

#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <vector>

namespace util {

    template <typename Container>
    void normalize(Container& c) {
        std::sort(c.begin(), c.end());
        c.erase(std::unique(c.begin(), c.end()), c.end());
    }

    inline std::vector<size_t> range(size_t st, size_t ed) {
        std::vector<size_t> v(ed - st + 1);
        std::iota(v.begin(), v.end(), st);
        return v;
    }

    template <class Container>
    bool hasIntersection(const Container& a, const Container& b) {
        const auto& small = (a.size() < b.size()) ? a : b;
        const auto& large = (a.size() < b.size()) ? b : a;

        std::unordered_set<typename Container::value_type> s;
        s.reserve(small.size());
        s.insert(small.begin(), small.end());

        for (const auto& x : large) {
            if (s.count(x))
                return true;
        }
        return false;
    }

} // namespace util
