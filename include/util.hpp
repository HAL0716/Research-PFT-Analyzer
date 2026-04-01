#pragma once

#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace util {
    using ull = unsigned long long;

    ull calcPower(ull base, ull exp) {
        ull result = 1;
        for (ull i = 0; i < exp; ++i)
            result *= base;
        return result;
    }

    template <typename Container>
    ull calcProduct(const Container& v) {
        ull prod = 1;
        for (const auto& x : v)
            prod *= x;
        return prod;
    }

    template <typename Container>
    void normalize(Container& c) {
        std::sort(c.begin(), c.end());
        c.erase(std::unique(c.begin(), c.end()), c.end());
    }

    std::vector<size_t> range(size_t st, size_t ed) {
        std::vector<size_t> v(ed - st);
        std::iota(v.begin(), v.end(), st);
        return v;
    }

    template <class Container>
    std::string join(const Container& c, std::string_view delim, std::string pre = "", std::string suf = "") {
        std::ostringstream result;
        if (!c.empty()) {
            auto it = c.begin();
            result << *it++;
            for (; it != c.end(); ++it)
                result << delim << *it;
        }
        return pre + result.str() + suf;
    }

    template <typename T>
    std::set<std::vector<T>> comb(const std::vector<T>& elems, size_t P, bool repeat, bool ordered) {
        std::set<std::vector<T>> result;
        std::vector<T> cur;
        std::vector<bool> used(elems.size(), false);

        auto dfs = [&](auto&& self, size_t start) -> void {
            if (cur.size() == P) {
                result.insert(cur);
                return;
            }

            if (ordered) {
                for (size_t i = 0; i < elems.size(); ++i) {
                    if (!repeat && used[i])
                        continue;

                    used[i] = true;
                    cur.push_back(elems[i]);

                    self(self, 0);

                    cur.pop_back();
                    used[i] = false;
                }
            } else {
                for (size_t i = start; i < elems.size(); ++i) {
                    cur.push_back(elems[i]);

                    self(self, repeat ? i : i + 1);

                    cur.pop_back();
                }
            }
        };

        dfs(dfs, 0);
        return result;
    }

    template <typename Container>
    auto combs(const Container& elems, size_t P) {
        using T = typename Container::value_type;
        std::vector<T> v(elems.begin(), elems.end());
        std::set<std::set<T>> result;
        for (const auto& vec : comb(v, P, false, false))
            result.insert(std::set<T>(vec.begin(), vec.end()));
        return result;
    }

    template <typename Container>
    auto perms(const Container& elems, size_t P) {
        using T = typename Container::value_type;
        std::vector<T> v(elems.begin(), elems.end());
        return comb(v, P, false, true);
    }

    template <typename Container>
    auto combs_r(const Container& elems, size_t P) {
        using T = typename Container::value_type;
        std::vector<T> v(elems.begin(), elems.end());
        return comb(v, P, true, false);
    }

    template <typename Container>
    auto perms_r(const Container& elems, size_t P) {
        using T = typename Container::value_type;
        std::vector<T> v(elems.begin(), elems.end());
        return comb(v, P, true, true);
    }

    template <typename OuterContainer, typename CurrentContainer>
    auto prod_impl(const OuterContainer& elems) {
        std::set<CurrentContainer> res;
        CurrentContainer cur;

        auto dfs = [&](auto&& self, auto it) -> void {
            if (it == elems.end()) {
                res.insert(cur);
                return;
            }

            for (const auto& v : *it) {
                if constexpr (requires { cur.push_back(v); }) {
                    cur.push_back(v);
                    self(self, std::next(it));
                    cur.pop_back();
                } else {
                    cur.insert(v);
                    self(self, std::next(it));
                    cur.erase(v);
                }
            }
        };

        dfs(dfs, elems.begin());
        return res;
    }

    template <typename OuterContainer>
    auto prod_V(const OuterContainer& elems) {
        using T = typename OuterContainer::value_type::value_type;
        return prod_impl<OuterContainer, std::vector<T>>(elems);
    }

    template <typename OuterContainer>
    auto prod_S(const OuterContainer& elems) {
        using T = typename OuterContainer::value_type::value_type;
        return prod_impl<OuterContainer, std::set<T>>(elems);
    }
} // namespace util
