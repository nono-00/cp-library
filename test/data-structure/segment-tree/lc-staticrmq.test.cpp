#define PROBLEM "https://judge.yosupo.jp/problem/staticrmq"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "nono/data-structure/segment-tree.hpp"

namespace nono {

template <class T>
struct Min {
    using value_type = T;
    static T op(T lhs, T rhs) {
        return std::min(lhs, rhs);
    }
    static T e() {
        return std::numeric_limits<T>::max();
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    const SegmentTree<Min<long long>> segtree(a);
    while (q--) {
        int l, r;
        std::cin >> l >> r;
        std::cout << segtree.prod(l, r) << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
