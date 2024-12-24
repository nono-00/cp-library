#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/challenges/sources/PCK/Prelim/0415?year=2019"

#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/contract.hpp"
#include "../../../nono/graph/two-edge-connected-components.hpp"
#include "../../../nono/tree/is-tree.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::vector<int> vs(n);
    for (int i = 0; i < n; i++) std::cin >> vs[i];
    //  std::vector<std::vector<int>> graph(n);
    std::vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        u--;
        v--;
        edges.emplace_back(u, v);
    }
    const auto graph = to_undirected_graph(n, edges);
    const auto groups = two_edge_connected_components(graph);
    const auto contracted_graph = contract(graph, groups);
    assert(is_tree(contracted_graph));
    std::vector<int> nodes(contracted_graph.size());
    for (int i = 0; i < std::ssize(groups); i++) {
        const auto& group = groups[i];
        for (int j: group) {
            nodes[i] += vs[j];
        }
    }
    int ans = 0;
    auto dfs = [&](auto self, int u, int p) -> int {
        std::vector<int> res;
        for (const auto& e: contracted_graph[u]) {
            if (e.to == p) continue;
            res.push_back(self(self, e.to, u));
        }
        std::sort(res.begin(), res.end(), std::greater<int>());
        if (res.size() >= 2) {
            ans = std::max(ans, nodes[u] + res[0] + res[1]);
            return nodes[u] + res[0];
        } else if (res.size() >= 1) {
            ans = std::max(ans, nodes[u] + res[0]);
            return nodes[u] + res[0];
        } else {
            ans = std::max(ans, nodes[u]);
            return nodes[u];
        }
    };
    dfs(dfs, 0, -1);
    std::cout << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
