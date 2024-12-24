#define PROBLEM "https://judge.yosupo.jp/problem/two_edge_connected_components"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/two-edge-connected-components.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }
    const auto graph = to_undirected_graph(n, edges);
    const auto groups = two_edge_connected_components(graph);
    std::cout << groups.size() << '\n';
    for (const auto& group: groups) {
        std::cout << group.size();
        for (auto v: group) {
            std::cout << ' ' << v;
        }
        std::cout << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
