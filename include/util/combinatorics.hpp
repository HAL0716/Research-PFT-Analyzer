#pragma once

#include <set>
#include <vector>

namespace util {

    class Combinatorics {
      public:
        template <typename Container>
        static auto combs(const Container& elems, size_t P) {
            using T = typename Container::value_type;
            auto tmp = impl(std::vector<T>(elems.begin(), elems.end()), P, false, false);

            std::set<std::set<T>> res;
            for (const auto& vec : tmp)
                res.emplace(vec.begin(), vec.end());
            return res;
        }

        template <typename Container>
        static auto perms(const Container& elems, size_t P) {
            using T = typename Container::value_type;
            return impl(std::vector<T>(elems.begin(), elems.end()), P, false, true);
        }

        template <typename Container>
        static auto combs_r(const Container& elems, size_t P) {
            using T = typename Container::value_type;
            return impl(std::vector<T>(elems.begin(), elems.end()), P, true, false);
        }

        template <typename Container>
        static auto perms_r(const Container& elems, size_t P) {
            using T = typename Container::value_type;
            return impl(std::vector<T>(elems.begin(), elems.end()), P, true, true);
        }

      private:
        template <typename T>
        static auto impl(const std::vector<T>& elems, size_t P, bool repeat, bool ordered) {
            std::set<std::vector<T>> res;
            std::vector<T> cur;
            std::vector<bool> used(elems.size());

            auto dfs = [&](auto&& self, size_t start) -> void {
                if (cur.size() == P) {
                    res.insert(cur);
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
            return res;
        }
    };

} // namespace util
