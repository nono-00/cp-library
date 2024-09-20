#define PROBLEM "https://judge.yosupo.jp/problem/point_add_range_sum"
#include <iostream>
#include <vector>

#include "nono/ds/segment-tree.hpp"

namespace nono {

template <class T>
struct Add {
    using Value = T;
    static T op(T lhs, T rhs) {
        return lhs + rhs;
    }
    static T e() {
        return 0;
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    SegmentTree<Add<long long>> segtree(a);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            long long x;
            std::cin >> p >> x;
            segtree.set(p, segtree.get(p) + x);
        } else {
            int l, r;
            std::cin >> l >> r;
            std::cout << segtree.prod(l, r) << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
