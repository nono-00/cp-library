#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "nono/ds/csr-array.hpp"
#include "nono/graph/base.hpp"
#include "nono/graph/is-bipartite.hpp"

///  brief : 二部マッチング. かなり速い. 多重辺があっても動くはず.
namespace nono {

//  使った辺の添字列を昇順で返す.
std::vector<int> bipartite_matching(int left, int right, std::vector<std::pair<int, int>> edges) {
    int m = edges.size();
    for (auto& [u, v]: edges) {
        assert(0 <= u && u < left);
        assert(0 <= v && v < right);
        v += left;
    }
    const CSRArray graph(left, edges);
    const int inf = std::numeric_limits<int>::max();
    std::vector<int> adj(left + right, -1);
    std::vector<int> dist(left + right, inf);
    std::queue<int> que;
    while (true) {
        //  free頂点からbfsして距離を求める
        std::fill(dist.begin(), dist.end(), inf);
        for (int i = 0; i < left; i++) {
            if (adj[i] == -1) {
                dist[i] = 0;
                que.push(i);
            }
        }
        while (!que.empty()) {
            int u = que.front();
            que.pop();
            for (auto v: graph[u]) {
                if (dist[v] == inf && adj[u] != v) {
                    dist[v] = dist[u] + 1;
                    if (adj[v] != -1) {
                        dist[adj[v]] = dist[v] + 1;
                        que.push(adj[v]);
                    }
                }
            }
        }
        bool reached = false;
        for (int i = left; i < left + right; i++) {
            reached |= dist[i] != inf && adj[i] == -1;
        }
        if (!reached) break;
        auto dfs = [&](auto&& self, int u) -> bool {
            assert(u < left);
            for (auto v: graph[u]) {
                if (dist[u] + 1 == dist[v]) {
                    dist[v] = inf;
                    if (adj[v] == -1 || self(self, adj[v])) {
                        adj[u] = v;
                        adj[v] = u;
                        return true;
                    }
                }
            }
            return false;
        };
        for (int i = 0; i < left; i++) {
            if (adj[i] == -1) {
                dfs(dfs, i);
            }
        }
    }
    std::vector<int> result;
    for (int i = 0; i < m; i++) {
        auto [u, v] = edges[i];
        if (adj[u] == v) {
            result.push_back(i);
            adj[u] = -1;
        }
    }
    return result;
}

template <class T>
std::vector<int> bipartite_matching(const Graph<T>& graph) {
    assert(is_bipartite(graph));
    int n = graph.size();
    int m = graph.edge_size();
    std::vector<int> mapping(n, -1);
    std::vector<bool> is_lefts(n);
    int left = 0, right = 0;
    auto dfs = [&](auto&& self, int u, bool is_left = true) -> void {
        is_lefts[u] = is_left;
        if (is_left) {
            mapping[u] = left++;
        } else {
            mapping[u] = right++;
        }
        for (auto e: graph[u]) {
            if (mapping[e.to] == -1) {
                self(self, e.to, is_left ^ true);
            }
        }
    };
    for (int i = 0; i < n; i++) {
        if (mapping[i] == -1) {
            dfs(dfs, i);
        }
    }
    std::vector<std::pair<int, int>> edges(m);
    for (int u = 0; u < n; u++) {
        if (!is_lefts[u]) continue;
        for (auto e: graph[u]) {
            edges[e.id] = {mapping[u], mapping[e.to]};
        }
    }
    return bipartite_matching(left, right, edges);
}

}  //  namespace nono
