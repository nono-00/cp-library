#define PROBLEM "https://judge.yosupo.jp/problem/shortest_path"
#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>

#include "nono/graph/csr-graph.hpp"
#include "nono/graph/dijkstra.hpp"
#include "nono/graph/edge.hpp"

namespace nono {

void solve() {
    int n, m, s, t;
    std::cin >> n >> m >> s >> t;
    std::vector<Edge<long long>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w, i);
    }
    CSRGraph graph(n, edges);
    auto dist = dijkstra(graph, s);
    if (dist[t] == std::numeric_limits<long long>::max()) {
        std::cout << -1 << std::endl;
        return;
    }

    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int lhs, int rhs) { return dist[lhs] > dist[rhs]; });
    order.push_back(s);
    std::vector<int> prev(n, -1);
    for (int i: order) {
        if (dist[i] == std::numeric_limits<long long>::max()) continue;
        for (const auto& e: graph[i]) {
            if (dist[i] + e.weight == dist[e.to]) {
                prev[e.to] = e.id;
            }
        }
    }

    int now = t;
    std::vector<std::pair<int, int>> ans;
    while (now != s) {
        int i = prev[now];
        ans.emplace_back(edges[i].from, edges[i].to);
        now = edges[i].from;
    }
    std::cout << dist[t] << ' ' << ans.size() << '\n';
    std::reverse(ans.begin(), ans.end());
    for (const auto& [l, r]: ans) {
        std::cout << l << ' ' << r << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
