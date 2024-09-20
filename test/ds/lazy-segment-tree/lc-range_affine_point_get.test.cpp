#define PROBLEM "https://judge.yosupo.jp/problem/range_affine_point_get"

#include <iostream>
#include <utility>
#include <vector>

#include "nono/ds/lazy-segment-tree.hpp"
#include "nono/math/modint.hpp"

namespace nono {

using Mint = nono::Modint998244353;

//  c * ((a * x + b)) + d
//
//  c * a * x + c * b + d

struct M {
    using Value = Mint;
    using Act = std::pair<Mint, Mint>;

    static Value op(Value lhs, Value rhs) {
        return lhs + rhs;
    }
    static Value e() {
        return 0;
    }
    static Value mapping(Act lhs, Value rhs) {
        return rhs * lhs.first + lhs.second;
    }
    static Act composition(Act lhs, Act rhs) {
        return {lhs.first * rhs.first, lhs.first * rhs.second + lhs.second};
    }
    static Act id() {
        return {1, 0};
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<Mint> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    LazySegmentTree<M> segtree(a);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r;
            Mint b, c;
            std::cin >> l >> r >> b >> c;
            segtree.apply(l, r, std::pair<Mint, Mint>(b, c));
        } else {
            int i;
            std::cin >> i;
            assert(0 <= i && i < n);
            std::cout << segtree.get(i) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
