#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/3/GRL_3_B"
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/bridges.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        if (u > v) std::swap(u, v);
        edges.emplace_back(u, v);
    }
    auto graph = to_undirected_graph(n, edges);

    std::vector<std::pair<int, int>> ans;
    for (auto i: bridges(graph)) {
        ans.emplace_back(edges[i].from, edges[i].to);
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
