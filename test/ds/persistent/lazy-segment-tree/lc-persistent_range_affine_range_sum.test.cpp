#define PROBLEM "https://judge.yosupo.jp/problem/persistent_range_affine_range_sum"
#include <iostream>
#include <vector>

#include "nono/ds/persistent/lazy-segment-tree.hpp"
#include "nono/math/modint.hpp"
#include "nono/structure/act-monoid.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    using Monoid = act_monoid::RangeAffineRangeSum<Mint>;
    using Segtree = persistent::LazySegmentTree<Monoid>;
    int n, q;
    std::cin >> n >> q;
    std::vector<Segtree> segtrees(q + 1, Segtree(n));
    for (int i = 0; i < n; i++) {
        Mint value;
        std::cin >> value;
        segtrees[0] = segtrees[0].set(i, value);
    }
    for (int i = 0; i < q; i++) {
        int t, k;
        std::cin >> t >> k;
        k++;
        if (t == 0) {
            int l, r;
            Mint b, c;
            std::cin >> l >> r >> b >> c;
            segtrees[i + 1] = segtrees[k].apply(l, r, {b, c});
        } else if (t == 1) {
            int s, l, r;
            std::cin >> s >> l >> r;
            s++;
            segtrees[i + 1] = segtrees[k].copy(l, r, segtrees[s]);
        } else {
            int l, r;
            std::cin >> l >> r;
            std::cout << segtrees[k].prod(l, r).sum << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}