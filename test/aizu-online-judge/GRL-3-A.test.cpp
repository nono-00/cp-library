#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/3/GRL_3_A"
#include <algorithm>
#include <iostream>
#include <vector>

#include "graph/articulation-points.hpp"
#include "graph/csr-graph.hpp"
#include "graph/edge.hpp"

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

    auto articulation_point = articulation_points(graph);
    std::sort(articulation_point.begin(), articulation_point.end());
    for (auto v: articulation_point) {
        std::cout << v << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
