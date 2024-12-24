#define PROBLEM "https://judge.yosupo.jp/problem/cycle_detection_undirected"
#include <algorithm>
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/cycle-detection.hpp"

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
    auto result = cycle_detection(graph);

    if (!result.has_cycle()) {
        std::cout << -1 << std::endl;
    } else {
        auto eid = result.edge_id();
        auto vid = result.vertex_id();
        int len = eid.size();
        std::cout << len << std::endl;
        for (int i = 0; i < len; i++) {
            std::cout << vid[i] << (i + 1 == len ? '\n' : ' ');
        }
        for (int i = 0; i < len; i++) {
            std::cout << eid[i] << (i + 1 == len ? '\n' : ' ');
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
