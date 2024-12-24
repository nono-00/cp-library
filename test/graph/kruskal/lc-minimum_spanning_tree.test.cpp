#define PROBLEM "https://judge.yosupo.jp/problem/minimum_spanning_tree"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/kruskal.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<WeightedEdge<long long>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    auto result = kruskal(n, edges);
    std::cout << result.min_cost() << '\n';
    auto edge_id = result.edge_id();
    for (int i = 0; i + 1 < n; i++) {
        std::cout << edge_id[i] << (i + 1 == n - 1 ? '\n' : ' ');
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}

