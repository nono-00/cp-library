#pragma once

#include <bit>
#include <vector>

#include "./union-find.hpp"

namespace nono {

///  # Range Union Find
class RangeUnionFind {
    static constexpr auto DUMMY{[](int, int, int) {
    }};

  public:
    RangeUnionFind(int n): n_(n), log_(std::bit_width((unsigned)n_)), ufs_(log_, UnionFind(n_)) {}

    ///  # leader(x)
    ///  O(log n)
    int leader(int x) {
        return ufs_[0].leader(x);
    };

    ///  # merge(lhs, rhs, width)
    ///  add edge (lhs, rhs), (lhs + 1, rhs + 1), ..., (lhs + width - 1, rhs + width - 1)
    ///  全体でO(n (log n)^2)
    template <class F = decltype(DUMMY)>
    void merge(int lhs, int rhs, int width, F f = DUMMY) {
        auto dfs = [&](auto&& self, int lhs, int rhs, int i) -> void {
            if (ufs_[i].same(lhs, rhs)) return;
            if (i != 0) {
                ufs_[i].merge(lhs, rhs);
                self(self, lhs, rhs, i - 1);
                self(self, lhs + (1 << (i - 1)), rhs + (1 << (i - 1)), i - 1);
            } else {
                lhs = ufs_[i].leader(lhs);
                rhs = ufs_[i].leader(rhs);
                ufs_[i].merge(lhs, rhs);
                f(lhs, rhs, ufs_[i].leader(lhs));
            }
        };
        if (width <= 0) return;
        int i = std::bit_width((unsigned)width) - 1;
        dfs(dfs, lhs, rhs, i);
        dfs(dfs, lhs + width - (1 << i), rhs + width - (1 << i), i);
    }

    ///  # same(lhs, rhs)
    ///  whether lhs and rhs are in same group
    ///  O(log n)
    bool same(int lhs, int rhs) {
        return ufs_[0].same(lhs, rhs);
    }

    ///  # size(x)
    ///  return |group x|
    ///  O(log n)
    int size(int x) {
        return ufs_[0].size(x);
    }

    ///  # groups()
    ///  O(n log n)
    std::vector<std::vector<int>> groups() {
        return ufs_[0].groups();
    }

  private:
    int n_;
    int log_;
    std::vector<UnionFind> ufs_;
};

}  //  namespace nono