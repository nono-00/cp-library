#define PROBLEM "https://judge.yosupo.jp/problem/point_set_range_composite"
#include <iostream>

#include "nono/ds/persistent/segment-tree.hpp"
#include "nono/math/modint.hpp"
#include "nono/structure/monoid.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    using Monoid = monoid::Composite<Mint>;
    using Segtree = persistent::SegmentTree<Monoid>;
    int n, q;
    std::cin >> n >> q;
    Segtree segtree(n);
    for (int i = 0; i < n; i++) {
        Mint a, b;
        std::cin >> a >> b;
        segtree = segtree.set(i, {a, b});
    }
    for (int i = 0; i < q; i++) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            Mint b, c;
            std::cin >> p >> b >> c;
            segtree = segtree.set(p, {b, c});
        } else {
            int l, r;
            Mint x;
            std::cin >> l >> r >> x;
            std::cout << segtree.prod(l, r).eval(x) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}