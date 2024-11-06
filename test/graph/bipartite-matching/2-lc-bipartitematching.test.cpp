#define PROBLEM "https://judge.yosupo.jp/problem/bipartitematching"
#include <cassert>
#include <iostream>
#include <vector>

#include "nono/graph/bipartite-matching.hpp"

namespace nono {

void solve() {
    int l, r, m;
    std::cin >> l >> r >> m;
    std::vector<Edge> edges(m);
    for (auto& [u, v, _]: edges) std::cin >> u >> v, v += l;
    auto result = bipartite_matching(to_undirected_graph(l + r, edges));
    std::cout << result.size() << '\n';
    for (auto i: result) std::cout << edges[i].from << ' ' << edges[i].to - l << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
