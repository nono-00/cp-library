#define PROBLEM "https://judge.yosupo.jp/problem/vertex_set_path_composite"
#include <iostream>
#include <vector>

#include "../../../nono/math/modint.hpp"
#include "../../../nono/structure/monoid.hpp"
#include "../../../nono/tree/productable-tree.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    using Monoid = act_monoid::ActMonoid<monoid::Composite<Mint>>;
    using Value = Monoid::Value;
    int n, q;
    std::cin >> n >> q;
    std::vector<Value> a(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i].a >> a[i].b;
    }
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 1; i < n; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }
    ProductableTree<Monoid, int> tree(n, edges, a);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            Mint c, d;
            std::cin >> p >> c >> d;
            tree.set(p, Value{c, d});
        } else {
            int u, v;
            Mint x;
            std::cin >> u >> v >> x;
            std::cout << tree.prod_path(u, v).eval(x) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
