#define PROBLEM "https://judge.yosupo.jp/problem/vertex_add_path_sum"
#include <iostream>
#include <vector>

#include "../../../nono/structure/monoid.hpp"
#include "../../../nono/tree/productable-tree.hpp"

namespace nono {

void solve() {
    using Monoid = monoid::Add<long long>;
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
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
            long long x;
            std::cin >> p >> x;
            tree.set(p, a[p] += x);
        } else {
            int u, v;
            std::cin >> u >> v;
            std::cout << tree.prod_path(u, v) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
