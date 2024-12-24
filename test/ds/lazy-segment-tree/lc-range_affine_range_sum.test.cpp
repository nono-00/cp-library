#define PROBLEM "https://judge.yosupo.jp/problem/range_affine_range_sum"

#include <iostream>
#include <vector>

#include "../../../nono/ds/lazy-segment-tree.hpp"
#include "../../../nono/math/modint.hpp"
#include "../../../nono/structure/act-monoid.hpp"

namespace nono {

void solve() {
    using Mint = nono::Modint998244353;
    using Monoid = act_monoid::RangeAffineRangeSum<Mint>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;
    int n, q;
    std::cin >> n >> q;
    std::vector<Mint> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    LazySegmentTree<Monoid> segtree(std::vector<Value>(a.begin(), a.end()));
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r;
            Mint b, c;
            std::cin >> l >> r >> b >> c;
            segtree.apply(l, r, Act(b, c));
        } else {
            int l, r;
            std::cin >> l >> r;
            std::cout << segtree.prod(l, r).sum << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
