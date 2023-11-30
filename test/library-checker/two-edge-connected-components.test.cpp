#define PROBLEM "https://judge.yosupo.jp/problem/two_edge_connected_components"
#include <iomanip>
#include <iostream>
#include <vector>

#include "graph/csr-graph.hpp"
#include "graph/edge.hpp"
#include "graph/two-edge-connected-components.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<Edge<int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v, 1, i);
    }
    const CSRGraph graph(n, edges);
    const auto groups = two_edge_connected_components(graph);
    std::cout << groups.size() << '\n';
    for (const auto& group: groups) {
        std::cout << group.size() << ' ';
        for (int i = 0; auto v: group) {
            std::cout << v << (i + 1 < (int)group.size() ? ' ' : '\n');
            i++;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
