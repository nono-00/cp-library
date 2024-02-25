#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/2/GRL_2_A"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/kruskal.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<WeightedEdge<int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    auto result = kruskal(n, edges);
    std::cout << result.min_cost() << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
