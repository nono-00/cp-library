#define PROBLEM "https://judge.yosupo.jp/problem/point_set_range_composite"
#include <iostream>
#include <vector>

#include "nono/ds/segment-tree.hpp"
#include "nono/math/modint.hpp"
#include "nono/structure/monoid.hpp"

namespace nono {

void solve() {
    using Mint = nono::Modint998244353;
    using Monoid = monoid::Composite<Mint>;
    using Value = Monoid::Value;
    int n, q;
    std::cin >> n >> q;
    std::vector<Value> lines(n);
    for (int i = 0; i < n; i++) {
        Mint a, b;
        std::cin >> a >> b;
        lines[i] = Value(a, b);
    }

    SegmentTree<Monoid> segtree(lines);

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            Mint c, d;
            std::cin >> p >> c >> d;
            segtree.set(p, Value(c, d));
        } else {
            int l, r;
            Mint x;
            std::cin >> l >> r >> x;
            std::cout << segtree.prod(l, r).eval(x) << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
