#define PROBLEM "https://judge.yosupo.jp/problem/lca"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/tree/heavy-light-decomposition.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 1; i < n; i++) {
        int p;
        std::cin >> p;
        edges.emplace_back(p, i);
    }
    HeavyLightDecomposition hld(to_undirected_graph(n, edges));

    while (q--) {
        int u, v;
        std::cin >> u >> v;
        std::cout << hld.lowest_common_ancester(u, v) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
