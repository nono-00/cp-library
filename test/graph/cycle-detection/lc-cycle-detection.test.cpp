#define PROBLEM "https://judge.yosupo.jp/problem/cycle_detection"
#include <iostream>
#include <vector>

#include "nono/graph/csr-graph.hpp"
#include "nono/graph/cycle-detection.hpp"
#include "nono/graph/edge.hpp"

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
    CSRGraph graph(n, edges);

    auto cycle = cycle_detection(graph);

    if (cycle.empty()) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << cycle.size() << std::endl;
        for (const auto& e: cycle) {
            std::cout << e.id << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
