#define PROBLEM "https://judge.yosupo.jp/problem/jump_on_tree"
#include <iostream>
#include <vector>

#include "../../../nono/tree/tree.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 1; i < n; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }
    Tree tree(n, edges);
    while (q--) {
        int u, v, i;
        std::cin >> u >> v >> i;
        if (tree.dist(u, v) < i) {
            std::cout << -1 << '\n';
        } else {
            std::cout << tree.jump(u, v, i) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
