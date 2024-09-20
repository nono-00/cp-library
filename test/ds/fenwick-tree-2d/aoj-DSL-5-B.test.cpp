#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/5/DSL_5_B"
#include <algorithm>
#include <iostream>

#include "nono/data-structure/fenwick-tree-2d.hpp"

namespace nono {

template <class T>
struct Add {
    using value_type = T;
    static T op(T lhs, T rhs) {
        return lhs + rhs;
    }
    static T e() {
        return T(0);
    }
    static T inv(T elem) {
        return -elem;
    }
};

void solve() {
    constexpr int N = 1005;
    FenwickTree2D<Add<int>> fenwick(N, N);
    int n;
    std::cin >> n;
    while (n--) {
        int lh, lw, rh, rw;
        std::cin >> lh >> lw >> rh >> rw;
        fenwick.apply(rh, rw, 1);
        fenwick.apply(rh, lw, -1);
        fenwick.apply(lh, rw, -1);
        fenwick.apply(lh, lw, 1);
    }
    int ans = 0;
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            ans = std::max(ans, fenwick.prod(i, j));
        }
    }
    std::cout << ans << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
