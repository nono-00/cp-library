#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/1/GRL_1_A"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/dijkstra.hpp"

namespace nono {

void solve() {
    int n, m, s;
    std::cin >> n >> m >> s;
    std::vector<WeightedEdge<long long>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int a, b;
        long long c;
        std::cin >> a >> b >> c;
        edges.emplace_back(a, b, c);
    }
    auto graph = to_directed_graph(n, edges);
    auto result = dijkstra(graph, s);
    for (int i = 0; i < n; i++) {
        if (result.invalid(i)) {
            std::cout << "INF" << '\n';
        } else {
            std::cout << result.dist(i) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
