#pragma once

#include <vector>
#include <algorithm>

namespace nono {

//  brief:
//  - 関節点列挙
//
//  complexity:
//  - O(V + E)
//
//  param:
//  - `graph`: 無向グラフ. 連結であるかどうかは問わない.
//
//  note:
//  - 非単純グラフでも動く（はず）
template <class GraphType>
std::vector<int> articulation_points(const GraphType& graph) {
    constexpr int NONE = -1;

    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> low(n);
    std::vector<int> result;
    int now = 0;

    auto dfs = [&](const auto& dfs, int u, int edge_id) -> void {
        bool add = false;
        order[u] = now++;
        low[u] = order[u];
        int child = 0;
        for (const auto& edge: graph[u]) {
            if (edge.id == edge_id) continue;
            if (order[edge.to] == NONE) {
                child++;
                dfs(dfs, edge.to, edge.id);
                low[u] = std::min(low[u], low[edge.to]);
                if (edge_id != NONE and order[u] <= low[edge.to] and not add) {
                    result.push_back(u);
                    add = true;
                }
            } else {
                low[u] = std::min(low[u], order[edge.to]);
            }
        }

        if (edge_id == NONE && child >= 2) {
            result.push_back(u);
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
