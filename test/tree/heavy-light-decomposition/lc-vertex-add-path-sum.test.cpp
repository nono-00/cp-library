#define PROBLEM "https://judge.yosupo.jp/problem/vertex_add_path_sum"
#include <iostream>
#include <vector>

#include "nono/data-structure/fenwick-tree.hpp"
#include "nono/graph/base.hpp"
#include "nono/tree/heavy-light-decomposition.hpp"

namespace nono {

struct Add {
    using value_type = long long;
    static long long op(long long lhs, long rhs) {
        return lhs + rhs;
    }
    static long long e() {
        return 0;
    }
    static long long inv(long long value) {
        return -value;
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u, v);
    }
    FenwickTree<Add> fen(n);
    HeavyLightDecomposition hld(to_undirected_graph(n, edges));
    for (int i = 0; i < n; i++) {
        fen.apply(hld.vertex(i), a[i]);
    }
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            long long x;
            std::cin >> p >> x;
            int e = hld.vertex(p);
            fen.apply(e, x);
        } else {
            int u, v;
            std::cin >> u >> v;
            long long ans = 0;
            for (auto&& [l, r]: hld.vertices_for_path(u, v)) {
                ans += fen.prod(l, r);
            }
            std::cout << ans << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
