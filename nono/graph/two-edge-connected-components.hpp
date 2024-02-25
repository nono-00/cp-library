#pragma once

#include <algorithm>
#include <vector>

#include "nono/graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - 二辺連結成分分解
//
//  complexity:
//  - O(V + E)
//
//  param:
//  - `graph`: 無向グラフ. 連結であるかどうかは問わない.
//
//  return:
//  - 連結成分ごとの頂点集合の配列
//
//  note:
//  - 非単純グラフでも動く（はず）
template <internal::IndexedGraph GraphType>
std::vector<std::vector<int>> two_edge_connected_components(const GraphType& graph) {
    constexpr int NONE = -1;

    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> low(n);
    std::vector<int> stack;
    std::vector<std::vector<int>> groups;
    int now = 0;

    auto dfs = [&](const auto& self, int u, int edge_id) -> void {
        order[u] = now++;
        low[u] = order[u];
        for (const auto& edge: graph[u]) {
            if (edge.id == edge_id) continue;
            if (order[edge.to] == NONE) {
                self(self, edge.to, edge.id);
                low[u] = std::min(low[u], low[edge.to]);
                if (order[u] < low[edge.to]) {
                    std::vector<int> group;
                    while (!stack.empty()) {
                        int v = stack.back();
                        if (order[u] >= low[v]) break;
                        stack.pop_back();
                        group.push_back(v);
                    }
                    groups.push_back(group);
                }
            } else {
                low[u] = std::min(low[u], order[edge.to]);
            }
        }
        stack.push_back(u);
    };

    for (int i = 0; i < n; i++) {
        if (order[i] == NONE) {
            dfs(dfs, i, NONE);
            groups.emplace_back(stack);
            stack.clear();
        }
    }
    return groups;
}

}  //  namespace nono
