#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/4/GRL_4_A"
#include <iostream>
#include <vector>

#include "nono/graph/csr-graph.hpp"
#include "nono/graph/edge.hpp"
#include "nono/graph/topological-sort.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<Edge<int>> edges;
    edges.reserve(m);

    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }

    const CSRGraph graph(n, edges);
    auto toposort = topological_sort(graph);
    if ((int)toposort.size() == n) {
        std::cout << 0 << std::endl;
    } else {
        std::cout << 1 << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
