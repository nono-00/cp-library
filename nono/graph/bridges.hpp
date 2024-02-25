#pragma once

#include <algorithm>
#include <vector>

#include "graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - 橋列挙
//
//  complexity:
//  - O(V + E)
//
//  param:
//  - `graph`: 無向グラフ. 連結非連結は問わない.
//
//  return:
//  - 橋となる辺の配列
//
//  note:
//  - 非単純グラフでも動く（はず）
//  - edgeにindexを持たせないと壊れる
//  - 橋: その辺を取り除くとグラフが非連結となるような辺.
template <internal::IndexedGraph GraphType>
auto bridges(const GraphType& graph) {
    using EdgeType = GraphType::EdgeType;
    constexpr int NONE = -1;

    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> low(n);
    std::vector<EdgeType> result;
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
                    result.push_back(edge);
                }
            } else {
                low[u] = std::min(low[u], order[edge.to]);
            }
        }
    };

    for (int i = 0; i < n; i++) {
        if (order[i] == NONE) {
            dfs(dfs, i, NONE);
        }
    }

    return result;
}

}  //  namespace nono
