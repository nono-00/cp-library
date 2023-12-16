#define PROBLEM "https://judge.yosupo.jp/problem/tree_diameter"
#include <iomanip>
#include <iostream>

#include "graph/edge.hpp"
#include "tree/diameter.hpp"
#include "tree/is-tree.hpp"
#include "tree/tree.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    Tree<Edge<long long>> graph(n);
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph.add_edge(Edge(u, v, w));
        graph.add_edge(Edge(v, u, w));
    }
    const auto info = diameter(graph);
    std::cout << info.dist << ' ' << info.path.size() + 1 << '\n';
    for (const auto& e: info.path) {
        std::cout << e.from << ' ';
    }
    std::cout << info.endpoints[1] << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
