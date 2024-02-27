#define PROBLEM "https://judge.yosupo.jp/problem/persistent_unionfind"
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "nono/data-structure/undo-unionfind.hpp"
#include "nono/graph/base.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<std::vector<std::tuple<int, int, int>>> query(q + 1);
    std::vector<WeightedEdge<std::pair<int, int>>> edges;
    int count = 0;
    for (int i = 1; i <= q; i++) {
        int t, k, u, v;
        std::cin >> t >> k >> u >> v;
        k++;
        if (t == 0) {
            edges.emplace_back(k, i, std::pair(u, v));
        } else {
            query[k].emplace_back(count, u, v);
            count++;
        }
    }
    std::vector<int> ans(count);
    auto graph = to_directed_graph(q + 1, edges);
    const int root = 0;
    UndoUnionFind uf(n);

    auto dfs = [&](auto self, int u) -> void {
        for (auto [i, v, w]: query[u]) {
            ans[i] = uf.same(v, w);
        }
        for (auto e: graph[u]) {
            auto [v, w] = e.weight;
            uf.merge(v, w);
            self(self, e.to);
            uf.undo();
        }
    };

    dfs(dfs, root);
    for (auto v: ans) {
        std::cout << v << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
