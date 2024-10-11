#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc351/tasks/abc351_g"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/math/modint.hpp"
#include "nono/structure/act-monoid.hpp"
#include "nono/tree/static-top-tree.hpp"

namespace nono {

using Mint = Modint998244353;

struct Monoid {
    using Value = std::pair<Mint, Mint>;
    using Act = bool;
    static Value op(Value lhs, Value rhs) {
        return {lhs.first + lhs.second * rhs.first, lhs.second * (rhs.first + rhs.second)};
    }
    static Value e() {
        return {0, 1};
    }
    static Value mapping(Act lhs, Value value) {
        if (lhs) return {0, value.first + value.second};
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        return lhs || rhs;
    }
    static Act id() {
        return false;
    }
};

void solve() {
    using Mint = Modint998244353;
    using Value = Monoid::Value;
    using Path = Monoid::Act;
    int n, q;
    std::cin >> n >> q;
    std::vector<WeightedEdge<bool>> edges;
    for (int i = 1; i < n; i++) {
        int p;
        std::cin >> p;
        p--;
        edges.emplace_back(p, i, false);
    }
    std::vector<Value> a(n);
    for (auto& v: a) std::cin >> v.first, v.second = 1;
    StaticTopTree<Monoid> tree(n, edges, a);
    while (q--) {
        int u;
        Mint x;
        std::cin >> u >> x;
        u--;
        tree.set_vertex(u, {x, 1});
        auto result = tree.all_prod();
        std::cout << result.first + result.second << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
