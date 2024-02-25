#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc137/tasks/abc137_e"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "nono/graph/bellman-ford.hpp"
#include "nono/graph/csr-graph.hpp"
#include "nono/graph/edge.hpp"

namespace nono {

void solve() {
    constexpr long long INF = std::numeric_limits<long long>::max();

    int n, m, p;
    std::cin >> n >> m >> p;
    std::vector<Edge<long long>> edges(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        u--;
        v--;
        edges[i] = Edge(u, v, w);
    }
    CSRGraph graph(n, edges);

    auto dist = bellman_ford(graph, 0);

    if (dist[n - 1] == -INF) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << std::max(0LL, -dist[n - 1]) << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
