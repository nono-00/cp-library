#define PROBLEM "https://judge.yosupo.jp/problem/static_range_sum"
#include <iostream>

#include "nono/data-structure/fenwick-tree.hpp"

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
    int n, q;
    std::cin >> n >> q;
    FenwickTree<Add<long long>> fenwick(n);
    for (int i = 0; i < n; i++) {
        long long v;
        std::cin >> v;
        fenwick.apply(i, v);
    }
    while (q--) {
        int l, r;
        std::cin >> l >> r;
        std::cout << fenwick.prod(l, r) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
