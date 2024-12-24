#define PROBLEM "https://judge.yosupo.jp/problem/range_affine_point_get"

#include <iostream>
#include <vector>

#include "../../../nono/ds/lazy-segment-tree.hpp"
#include "../../../nono/math/modint.hpp"
#include "../../../nono/structure/act-monoid.hpp"

namespace nono {

using Mint = nono::Modint998244353;

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<Mint> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    using Monoid = act_monoid::RangeAffineRangeSum<Mint, unsigned>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;
    LazySegmentTree<Monoid> segtree(std::vector<Value>(a.begin(), a.end()));
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r;
            Mint b, c;
            std::cin >> l >> r >> b >> c;
            segtree.apply(l, r, Act{b, c});
        } else {
            int i;
            std::cin >> i;
            assert(0 <= i && i < n);
            std::cout << segtree.get(i).sum << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
