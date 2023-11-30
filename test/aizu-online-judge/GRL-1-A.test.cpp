#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/1/GRL_1_A"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "graph/csr-graph.hpp"
#include "graph/dijkstra.hpp"
#include "graph/edge.hpp"

namespace nono {

void solve() {
    constexpr long long INF = std::numeric_limits<long long>::max();

    int n, m, s;
    std::cin >> n >> m >> s;
    std::vector<Edge<long long>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int a, b;
        long long c;
        std::cin >> a >> b >> c;
        edges.emplace_back(a, b, c);
    }
    CSRGraph graph(n, edges);

    for (auto dist: dijkstra(graph, s)) {
        if (dist == INF) {
            std::cout << "INF" << std::endl;
        } else {
            std::cout << dist << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
