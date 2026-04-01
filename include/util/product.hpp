#pragma once

#include <set>
#include <vector>

namespace util {

    class Product {
      public:
        template <typename Container>
        static auto asVec(const Container& elems) {
            using T = typename Container::value_type::value_type;
            return impl<Container, std::vector<T>>(elems);
        }

        template <typename Container>
        static auto asSet(const Container& elems) {
            using T = typename Container::value_type::value_type;
            return impl<Container, std::set<T>>(elems);
        }

      private:
        template <typename Container, typename ChildContainer>
        static std::set<ChildContainer> impl(const Container& elems) {
            std::set<ChildContainer> res;
            ChildContainer cur;

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
    };

} // namespace util
