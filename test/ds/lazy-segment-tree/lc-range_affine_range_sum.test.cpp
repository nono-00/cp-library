#define PROBLEM "https://judge.yosupo.jp/problem/range_affine_range_sum"

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
    using Value = std::pair<Mint, int>;
    using Act = std::pair<Mint, Mint>;

    static Value op(Value lhs, Value rhs) {
        return {lhs.first + rhs.first, lhs.second + rhs.second};
    }
    static Value e() {
        return {0, 0};
    }
    static Value mapping(Act lhs, Value rhs) {
        return {lhs.first * rhs.first + lhs.second * rhs.second, rhs.second};
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
    std::vector<std::pair<Mint, int>> a(n, {0, 1});
    for (int i = 0; i < n; i++) std::cin >> a[i].first;
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
            int l, r;
            std::cin >> l >> r;
            std::cout << segtree.prod(l, r).first << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
