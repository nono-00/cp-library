#define PROBLEM "https://judge.yosupo.jp/problem/cycle_detection"
#include <iostream>
#include <vector>

#include "../../../nono/graph/cycle-detection.hpp"
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
    auto graph = to_directed_graph(n, edges);
    auto result = cycle_detection(graph);

    if (!result.has_cycle()) {
        std::cout << -1 << std::endl;
    } else {
        auto eid = result.edge_id();
        int len = eid.size();
        std::cout << len << std::endl;
        for (int i = 0; i < len; i++) {
            std::cout << eid[i] << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
