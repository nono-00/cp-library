#define PROBLEM "https://judge.yosupo.jp/problem/scc"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/strongly-connected-components.hpp"

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
    const auto graph = to_directed_graph(n, edges);
    auto groups = strongly_connected_components(graph);
    std::cout << groups.size() << '\n';
    for (const auto& group: groups) {
        std::cout << group.size();
        for (int v: group) {
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
