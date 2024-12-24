#define IGNORE
#define PROBELM "https://atcoder.jp/contests/abc294/tasks/abc294_g"
#include <iostream>
#include <vector>

#include "../../../nono/ds/fenwick-tree.hpp"
#include "../../../nono/graph/base.hpp"
#include "../../../nono/tree/heavy-light-decomposition.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<WeightedEdge<long long>> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        u--;
        v--;
        edges.emplace_back(u, v, w);
    }
    FenwickTree<long long> fen(n);
    HeavyLightDecomposition hld(to_undirected_graph(n, edges));
    for (const auto& e: edges) {
        fen.add(hld.edge(e.from, e.to), e.weight);
    }

    int q;
    std::cin >> q;
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 1) {
            int i;
            long long w;
            std::cin >> i >> w;
            i--;
            int e = hld.edge(edges[i].from, edges[i].to);
            fen.add(e, w - fen.sum(e, e + 1));
        } else {
            int u, v;
            std::cin >> u >> v;
            u--;
            v--;
            long long ans = 0;
            for (auto&& [l, r]: hld.edges_for_path(u, v)) {
                ans += fen.sum(l, r);
            }
            std::cout << ans << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
