#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/1/GRL_1_B"
#include <iostream>
#include <limits>
#include <vector>

#include "graph/bellman-ford.hpp"
#include "graph/csr-graph.hpp"
#include "graph/edge.hpp"

namespace nono {

void solve() {
    constexpr int INF = std::numeric_limits<int>::max();

    int n, m, r;
    std::cin >> n >> m >> r;
    std::vector<Edge<int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    CSRGraph graph(n, edges);

    auto dist = bellman_ford(graph, r);

    bool negative = false;
    for (int i = 0; i < n; i++) {
        negative |= (dist[i] == -INF);
    }

    if (negative) {
        std::cout << "NEGATIVE CYCLE" << std::endl;
    } else {
        for (auto d: dist) {
            if (d == INF) {
                std::cout << "INF" << std::endl;
            } else {
                std::cout << d << std::endl;
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
