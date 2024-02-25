#define PROBLEM "https://judge.yosupo.jp/problem/shortest_path"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/dijkstra.hpp"

namespace nono {

void solve() {
    int n, m, s, t;
    std::cin >> n >> m >> s >> t;
    std::vector<WeightedEdge<long long>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    auto graph = to_directed_graph(n, edges);
    auto result = dijkstra(graph, s);

    if (result.invalid(t)) {
        std::cout << -1 << '\n';
        return;
    }
    auto path = result.path(t);
    int length = path.size();
    std::cout << result.dist(t) << ' ' << length - 1 << '\n';
    for (int i = 0; i + 1 < length; i++) {
        std::cout << path[i] << ' ' << path[i + 1] << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
