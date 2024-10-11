#define PROBLEM "https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum_fixed_root"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/math/modint.hpp"
#include "nono/structure/act-monoid.hpp"
#include "nono/tree/static-top-tree.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    using Monoid = act_monoid::RangeAffineRangeSum<Mint>;
    using Value = Monoid::Value;
    using Path = Monoid::Act;
    int n, q;
    std::cin >> n >> q;
    std::vector<Value> a(n);
    for (auto& v: a) std::cin >> v.sum, v.num = 1;
    std::vector<WeightedEdge<Path>> edges(n - 1);
    for (auto& [u, v, w]: edges) std::cin >> u >> v >> w.a >> w.b;
    StaticTopTree<Monoid> tree(n, edges, a);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int u;
            Mint x;
            std::cin >> u >> x;
            tree.set_vertex(u, Value{x, 1});
        } else {
            int i;
            Mint x, y;
            std::cin >> i >> x >> y;
            tree.set_edge(i, Path{x, y});
        }
        std::cout << tree.all_prod().sum << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
