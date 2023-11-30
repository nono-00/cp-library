#define PROBLEM "https://judge.yosupo.jp/problem/vertex_add_subtree_sum"
#include <iomanip>
#include <iostream>

#include "data-structure/fenwick-tree.hpp"
#include "graph/csr-graph.hpp"
#include "graph/heavy-light-decomposition.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    std::vector<Edge<long long>> edges;
    edges.reserve(n - 1);
    for (int i = 1; i < n; i++) {
        int p;
        std::cin >> p;
        edges.emplace_back(p, i);
    }
    FenwickTree<long long> fen(n);
    HeavyLightDecomposition hld(CSRGraph(n, edges));
    for (int i = 0; i < n; i++) {
        fen.add(hld.vertex(i), a[i]);
    }

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int u;
            long long x;
            std::cin >> u >> x;
            fen.add(hld.vertex(u), x);
        } else {
            int u;
            std::cin >> u;
            auto [l, r] = hld.subtree(u);
            long long ans = fen.sum(l, r);
            std::cout << ans << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
