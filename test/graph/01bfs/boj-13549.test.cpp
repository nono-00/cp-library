#include <iostream>
#include <vector>

#include "../../../nono/graph/01bfs.hpp"
#include "../../../nono/graph/base.hpp"

namespace nono {

void solve() {
    int s, e;
    std::cin >> s >> e;
    std::vector<WeightedEdge<int>> edges;
    const int n = 200005;
    for (int i = 0; i < n; i++) {
        if (i - 1 >= 0) edges.emplace_back(i, i - 1, 1);
        if (i + 1 < n) edges.emplace_back(i, i + 1, 1);
        if (2 * i < n) edges.emplace_back(i, 2 * i, 0);
    }
    auto graph = to_directed_graph(n, edges);
    std::cout << bfs01(graph, s).dist(e) << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
