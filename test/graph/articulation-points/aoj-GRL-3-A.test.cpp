#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/3/GRL_3_A"
#include <iostream>
#include <vector>

#include "../../../nono/graph/articulation-points.hpp"
#include "../../../nono/graph/base.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }
    auto graph = to_undirected_graph(n, edges);

    auto articulation_point = articulation_points(graph);
    for (auto v: articulation_point) {
        std::cout << v << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
