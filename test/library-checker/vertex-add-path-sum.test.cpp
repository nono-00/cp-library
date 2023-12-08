#define PROBLEM "https://judge.yosupo.jp/problem/vertex_add_path_sum"
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
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
        edges.emplace_back(v, u);
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
            int p;
            long long x;
            std::cin >> p >> x;
            int e = hld.vertex(p);
            fen.add(e, x);
        } else {
            int u, v;
            std::cin >> u >> v;
            long long ans = 0;
            for (auto&& [l, r]: hld.path(u, v)) {
                ans += fen.sum(l, r);
            }
            int lca = hld.vertex(hld.lowest_common_ancester(u, v));
            ans += fen.sum(lca, lca + 1);
            std::cout << ans << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
