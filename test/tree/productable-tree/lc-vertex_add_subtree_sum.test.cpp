#define PROBLEM "https://judge.yosupo.jp/problem/vertex_add_subtree_sum"
#include <iostream>
#include <vector>

#include "../../../nono/structure/monoid.hpp"
#include "../../../nono/tree/productable-tree.hpp"

namespace nono {

void solve() {
    using Monoid = monoid::Add<long long>;
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 1; i < n; i++) {
        int p;
        std::cin >> p;
        edges.emplace_back(p, i);
    }
    ProductableTree<Monoid, int> tree(n, edges, a);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            long long x;
            std::cin >> p >> x;
            tree.set(p, a[p] += x);
        } else {
            int u;
            std::cin >> u;
            std::cout << tree.prod_subtree(u) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
