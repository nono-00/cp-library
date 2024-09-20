#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/2235"

#include <iostream>
#include <vector>

#include "nono/data-structure/offline-dynamic-connectivity.hpp"
#include "nono/data-structure/undo-unionfind.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<std::pair<int, int>> querys;
    std::vector<std::pair<int, int>> vertex;
    std::vector<int> ans(q, -1);
    for (int i = 0; i < q; i++) {
        int t, u, v;
        std::cin >> t >> u >> v;
        vertex.emplace_back(u, v);
        if (t <= 2) {
            querys.emplace_back(u, v);
        } else {
            querys.emplace_back(0, 0);
            ans[i] = 0;
        }
    }
    auto calc = [&](int i, UndoUnionFind& uf) {
        if (ans[i] == 0) {
            ans[i] = uf.same(vertex[i].first, vertex[i].second);
        }
    };
    offline_dynamic_connectivity(n, querys, calc);
    for (int i = 0; i < q; i++) {
        if (ans[i] != -1) std::cout << (ans[i] ? "YES" : "NO") << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
