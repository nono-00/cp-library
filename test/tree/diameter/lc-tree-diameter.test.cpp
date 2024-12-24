#define PROBLEM "https://judge.yosupo.jp/problem/tree_diameter"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/tree/diameter.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<WeightedEdge<long long>> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    const auto graph = to_undirected_graph(n, edges);
    assert(is_tree(graph));
    const auto result = diameter(graph);
    const auto vid = result.vertex_id();
    std::cout << result.dist() << ' ' << vid.size() << '\n';
    for (int i = 0; i < std::ssize(vid); i++) {
        std::cout << vid[i] << (i + 1 == std::ssize(vid) ? '\n' : ' ');
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
