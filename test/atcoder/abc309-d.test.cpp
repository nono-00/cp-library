#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc309/tasks/abc309_d"
#include <algorithm>
#include <iostream>
#include <vector>

#include "graph/bfs.hpp"
#include "graph/csr-graph.hpp"
#include "graph/edge.hpp"

namespace nono {

void solve() {
    int n1, n2, m;
    std::cin >> n1 >> n2 >> m;
    std::vector<Edge<int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        u--;
        v--;
        edges.emplace_back(u, v);
    }
    CSRGraph graph(n1 + n2, edges);

    auto dist1 = bfs(graph, 0);
    auto dist2 = bfs(graph, n1 + n2 - 1);
    std::cout << *std::max_element(dist1.begin(), dist1.begin() + n1)
                     + *std::max_element(dist2.begin() + n1, dist2.end()) + 1
              << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
