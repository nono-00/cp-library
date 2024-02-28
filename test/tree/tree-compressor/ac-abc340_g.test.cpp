#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc340/tasks/abc340_g"
#include <bits/stdc++.h>

#include "nono/graph/base.hpp"
#include "nono/math/modint.hpp"
#include "nono/tree/tree-compressor.hpp"

namespace nono {

void solve() {
    using Mint = nono::Modint998244353;
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        std::cin >> u >> v;
        u--;
        v--;
        edges.emplace_back(u, v);
    }
    TreeCompressor tree_compressor(to_undirected_graph(n, edges));
    std::vector divide_by_color(n, std::vector<int>());
    for (int i = 0; i < n; i++) {
        a[i]--;
        divide_by_color[a[i]].push_back(i);
    }
    Mint ans = 0;
    for (int c = 0; c < n; c++) {
        if (divide_by_color[c].empty()) continue;
        auto result = tree_compressor.compress(divide_by_color[c]);
        int m = result.graph.size();
        const auto& graph = result.graph;
        const auto& mapping = result.mapping;

        std::vector<Mint> dp(m);
        auto dfs = [&](auto self, int u, int p) -> Mint {
            dp[u] = 1;
            for (const auto& e: graph[u]) {
                if (e.to == p) continue;
                dp[u] *= (self(self, e.to, u) + 1);
            }
            Mint diff = 0;
            if (a[mapping[u]] != c) {
                dp[u] -= 1;
                for (const auto& e: graph[u]) {
                    if (e.to == p) continue;
                    dp[u] -= dp[e.to];
                    diff += dp[e.to];
                }
            }
            ans += dp[u];
            dp[u] += diff;
            return dp[u];
        };
        dfs(dfs, 0, -1);
    }
    std::cout << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
