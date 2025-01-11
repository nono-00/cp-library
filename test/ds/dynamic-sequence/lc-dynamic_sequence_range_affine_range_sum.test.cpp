#define PROBLEM "https://judge.yosupo.jp/problem/dynamic_sequence_range_affine_range_sum"
#include <iostream>

#include "../../../nono/ds/dynamic-sequence.hpp"
#include "../../../nono/math/modint.hpp"
#include "../../../nono/structure/act-monoid.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    int n, q;
    std::cin >> n >> q;
    DynamicSequence<act_monoid::RangeAffineRangeSum<Mint>> seq;
    for (int i = 0; i < n; i++) {
        Mint value;
        std::cin >> value;
        seq.push_back(value);
    }
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int i;
            Mint x;
            std::cin >> i >> x;
            seq.insert(i, x);
        } else if (t == 1) {
            int i;
            std::cin >> i;
            seq.erase(i);
        } else if (t == 2) {
            int l, r;
            std::cin >> l >> r;
            seq.reverse(l, r);
        } else if (t == 3) {
            int l, r;
            Mint b, c;
            std::cin >> l >> r >> b >> c;
            seq.apply(l, r, {b, c});
        } else {
            int l, r;
            std::cin >> l >> r;
            std::cout << seq.prod(l, r).sum << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
