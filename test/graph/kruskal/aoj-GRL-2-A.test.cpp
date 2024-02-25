#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/2/GRL_2_A"
#include <algorithm>
#include <iostream>
#include <vector>

#include "nono/graph/edge.hpp"
#include "nono/graph/kruskal.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<Edge<int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    std::sort(edges.begin(), edges.end(),
              [](const Edge<int>& lhs, const Edge<int>& rhs) { return lhs.weight < rhs.weight; });
    const auto& [cost, _] = kruskal(n, edges);
    std::cout << cost << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
