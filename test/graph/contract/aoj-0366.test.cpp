#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/challenges/sources/PCK/Prelim/0366?year=2017"

#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/contract.hpp"
#include "../../../nono/graph/strongly-connected-components.hpp"

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
    const auto graph = to_directed_graph(n, edges);
    const auto groups = strongly_connected_components(graph);
    const auto contracted_graph = contract(graph, groups);
    int in = 0;
    int out = 0;
    std::vector<int> indeg(contracted_graph.size()), outdeg(contracted_graph.size());
    for (int i = 0; i < (int)contracted_graph.size(); i++) {
        outdeg[i] = contracted_graph[i].size();
        for (const auto& e: contracted_graph[i]) {
            indeg[e.to]++;
        }
    }
    for (int i = 0; i < (int)contracted_graph.size(); i++) {
        if (indeg[i] == 0) in++;
        if (outdeg[i] == 0) out++;
    }
    if (contracted_graph.size() == 1) {
        std::cout << 0 << std::endl;
    } else {
        std::cout << std::max(in, out) << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
