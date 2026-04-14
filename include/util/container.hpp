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

    template <typename Container>
    bool hasIntersection(const Container& a, const Container& b) {
        const auto& small = (a.size() < b.size()) ? a : b;
        const auto& large = (a.size() < b.size()) ? b : a;

        std::unordered_set<typename Container::value_type> s;
        s.reserve(small.size());
        s.insert(small.begin(), small.end());

        for (const auto& x : large)
            if (s.count(x))
                return true;
        return false;
    }

    template <typename Container>
    bool isSubset(const Container& a, const Container& b) {
        std::unordered_set<typename Container::value_type> sb;
        sb.reserve(b.size());
        sb.insert(b.begin(), b.end());

        for (const auto& x : a)
            if (!sb.count(x))
                return false;

        return true;
    }

    template <typename Container>
    Container setUnion(const Container& a, const Container& b) {
        using T = typename Container::value_type;

        std::unordered_set<T> s;
        s.reserve(a.size() + b.size());

        s.insert(a.begin(), a.end());
        s.insert(b.begin(), b.end());

        return Container(s.begin(), s.end());
    }

    template <typename Container>
    Container difference(const Container& a, const Container& b) {
        std::unordered_set<typename Container::value_type> sb;
        sb.reserve(b.size());
        sb.insert(b.begin(), b.end());

        Container result;
        for (const auto& x : a)
            if (!sb.count(x))
                result.insert(result.end(), x);
        return result;
    }

} // namespace util
