#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc256/tasks/abc256_h"
#include <iostream>
#include <vector>

#include "nono/ds/segment-tree-beats.hpp"
#include "nono/structure/beats-monoid.hpp"

namespace nono {

void solve() {
    using Monoid = beats_monoid::RangeDivideUpdateRangeSum;
    using Value = Monoid::Value;
    using Act = Monoid::Act;
    int n, q;
    std::cin >> n >> q;
    std::vector<unsigned long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    SegmentTreeBeats<Monoid> segtree(std::vector<Value>(a.begin(), a.end()));
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 1) {
            int l, r;
            unsigned long long b;
            std::cin >> l >> r >> b;
            l--;
            segtree.apply(l, r, Act::divide(b));
        } else if (t == 2) {
            int l, r;
            unsigned long long b;
            std::cin >> l >> r >> b;
            l--;
            segtree.apply(l, r, Act::update(b));
        } else {
            int l, r;
            std::cin >> l >> r;
            l--;
            std::cout << segtree.prod(l, r).sum << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}

