#define PROBLEM "https://judge.yosupo.jp/problem/point_add_rectangle_sum"
#include <algorithm>
#include <iostream>

#include "nono/ds/dynamic-segment-tree-2d.hpp"

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
    DynamicSegmentTree2D<Add<long long>, unsigned> segtree;
    auto put = [&](unsigned x1, unsigned y1, long long diff) {
        auto val = segtree.get(x1, y1);
        segtree.set(x1, y1, val + diff);
    };
    for (int i = 0; i < n; i++) {
        unsigned x, y;
        long long w;
        std::cin >> x >> y >> w;
        put(x, y, w);
    }
    for (int i = 0; i < q; i++) {
        int t;
        std::cin >> t;
        if (t == 0) {
            unsigned x, y;
            long long w;
            std::cin >> x >> y >> w;
            put(x, y, w);
        } else {
            unsigned l, d, r, u;
            std::cin >> l >> d >> r >> u;
            std::cout << segtree.prod(l, d, r, u) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}

