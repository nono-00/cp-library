#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/0439"

#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/bfs.hpp"
#include "../../../nono/tree/tree-compressor.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<int> color(n);
    for (int i = 0; i < n; i++) std::cin >> color[i];
    for (int i = 0; i < n; i++) color[i]--;
    std::vector<Edge> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; i++) {
        int u, v;
        std::cin >> u >> v;
        u--;
        v--;
        edges.emplace_back(u, v);
    }
    const auto g = to_undirected_graph(n, edges);
    TreeCompressor tree_compressor(g);
    auto depth = bfs(g, 0).raw();

    std::vector<std::vector<int>> vertexs(n);
    for (int i = 0; i < n; i++) {
        vertexs[color[i]].push_back(i);
    }

    const int INF = 1e9;
    std::vector<int> ans(n, INF);
    for (int c = 0; c < n; c++) {
        if (vertexs[c].empty()) continue;
        auto result = tree_compressor.compress(vertexs[c]);
        const auto& graph = result.graph;
        assert(is_tree(graph));
        const auto& mapping = result.mapping;
        const int root = 0;
        int m = graph.size();
        std::vector<int> dp(m, INF);
        auto dist = [&](int u, int v) {
            return std::abs(depth[mapping[u]] - depth[mapping[v]]);
        };

        {
            auto dfs = [&](auto self, int u, int p) -> int {
                for (const auto& e: graph[u]) {
                    if (e.to == p) continue;
                    dp[u] = std::min(dp[u], self(self, e.to, u) + dist(u, e.to));
                }
                if (color[mapping[u]] == c) dp[u] = 0;
                return dp[u];
            };
            dfs(dfs, root, -1);
        }
        {
            auto dfs = [&](auto self, int u, int p, int parent_value) -> void {
                if (color[mapping[u]] == c) {
                    for (const auto& e: graph[u]) {
                        if (e.to == p) {
                            ans[mapping[u]] = std::min(ans[mapping[u]], parent_value + dist(u, e.to));
                        } else {
                            ans[mapping[u]] = std::min(ans[mapping[u]], dp[e.to] + dist(u, e.to));
                        }
                    }
                }

                const auto& adj = graph[u];
                int adj_size = adj.size();
                std::vector<int> prefix(adj_size + 1, INF), suffix(adj_size + 1, INF);
                for (int i = 0; i < adj_size; i++) {
                    const auto& e = adj[i];
                    if (e.to == p) {
                        prefix[i + 1] = std::min(prefix[i], parent_value + dist(u, e.to));
                    } else {
                        prefix[i + 1] = std::min(prefix[i], dp[e.to] + dist(u, e.to));
                    }
                }
                for (int i = 0; i < adj_size; i++) {
                    int j = adj_size - i - 1;
                    const auto& e = adj[j];
                    if (e.to == p) {
                        suffix[i + 1] = std::min(suffix[i], parent_value + dist(u, e.to));
                    } else {
                        suffix[i + 1] = std::min(suffix[i], dp[e.to] + dist(u, e.to));
                    }
                }
                std::reverse(suffix.begin(), suffix.end());
                for (int i = 0; i < adj_size; i++) {
                    const auto& e = adj[i];
                    if (e.to == p) continue;
                    int value = std::min(prefix[i], suffix[i + 1]);
                    if (color[mapping[u]] == c) value = 0;
                    self(self, e.to, u, value);
                }
            };
            dfs(dfs, root, -1, -1);
        }
    }

    for (auto v: ans) {
        std::cout << v << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
