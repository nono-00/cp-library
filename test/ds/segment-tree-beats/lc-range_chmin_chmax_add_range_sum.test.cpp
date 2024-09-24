#define PROBLEM "https://judge.yosupo.jp/problem/range_chmin_chmax_add_range_sum"
#include <iostream>
#include <vector>

#include "nono/ds/segment-tree-beats.hpp"
#include "nono/structure/beats-monoid.hpp"

namespace nono {

void solve() {
    using Monoid = beats_monoid::RangeChminChmaxAddRangeSum<long long>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    SegmentTreeBeats<Monoid> segtree(std::vector<Value>(a.begin(), a.end()));
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r;
            long long b;
            std::cin >> l >> r >> b;
            segtree.apply(l, r, Act::chmin(b));
        } else if (t == 1) {
            int l, r;
            long long b;
            std::cin >> l >> r >> b;
            segtree.apply(l, r, Act::chmax(b));
        } else if (t == 2) {
            int l, r;
            long long b;
            std::cin >> l >> r >> b;
            segtree.apply(l, r, Act::add(b));
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
