#define PROBLEM "https://judge.yosupo.jp/problem/bipartitematching"
#include <iostream>
#include <utility>
#include <vector>
#include <cassert>

#include "nono/graph/bipartite-matching.hpp"
#include "nono/graph/is-bipartite.hpp"

namespace nono {

void solve() {
    int l, r, m;
    std::cin >> l >> r >> m;
    std::vector<std::pair<int, int>> edges(m);
    for (auto& [u, v]: edges) std::cin >> u >> v;
    auto result = bipartite_matching(l, r, edges);
    std::cout << result.size() << '\n';
    for (auto i: result) std::cout << edges[i].first << ' ' << edges[i].second << '\n';
    std::vector<Edge> nono_edges(m);
    for (int i = 0; i < m; i++) {
        nono_edges[i].from = edges[i].first;
        nono_edges[i].to = edges[i].second + l;
    }
    assert(is_bipartite(to_undirected_graph(l + r, nono_edges)));
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
