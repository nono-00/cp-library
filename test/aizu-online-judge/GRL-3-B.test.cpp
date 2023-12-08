#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/3/GRL_3_B"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

#include "graph/bridges.hpp"
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
        if (u > v) std::swap(u, v);
        edges.emplace_back(u, v, 1, i);
        edges.emplace_back(v, u, 1, i);
    }
    CSRGraph graph(n, edges);

    std::vector<std::pair<int, int>> ans;
    for (const auto& edge: bridges(graph)) {
        if (edge.from < edge.to) {
            ans.emplace_back(edge.from, edge.to);
        } else {
            ans.emplace_back(edge.to, edge.from);
        }
    }
    std::sort(ans.begin(), ans.end());
    for (auto [s, t]: ans) {
        std::cout << s << ' ' << t << '\n';
    }
}

};  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
