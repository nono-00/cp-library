#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

///  brief : 強連結成分分解する. 単純非単純を問わない.

//  brief:
//  - 強連結成分分解
//
//  complexity:
//  - O(V + E)
//
//  param:
//  - `graph`: 有向グラフ. 単純非単純を問わない
//
//  return:
//  - 強連結成分ごとの頂点配列をトポロジカルソートした配列
//
//  note:
//  - Tarjan's algorithmを使用している.
template <class T>
std::vector<std::vector<int>> strongly_connected_components(const Graph<T>& graph) {
    assert(graph.is_directed());
    constexpr int NONE = -1;

    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> group_ids(n, NONE);
    std::vector<int> count(n);
    std::vector<int> history;
    history.reserve(n);

    int now = 0;
    int group_id = 0;

    auto dfs = [&](const auto& self, int u) -> int {
        int lowlink = order[u] = now++;
        history.push_back(u);

        for (const auto& edge: graph[u]) {
            if (order[edge.to] == NONE) {
                lowlink = std::min(lowlink, self(self, edge.to));
            } else if (group_ids[edge.to] == NONE) {
                lowlink = std::min(lowlink, order[edge.to]);
            }
        }

        if (lowlink == order[u]) {
            while (!history.empty()) {
                int v = history.back();
                if (order[v] < order[u]) break;
                history.pop_back();
                group_ids[v] = group_id;
                count[group_id]++;
            }
            group_id++;
        }

        return lowlink;
    };

    for (int i = 0; i < n; i++) {
        if (order[i] == NONE) {
            dfs(dfs, i);
        }
    }

    std::vector<std::vector<int>> groups(group_id);
    for (int i = 0; i < group_id; i++) {
        groups[group_id - i - 1].reserve(count[i]);
    }
    for (int i = 0; i < n; i++) {
        groups[group_id - group_ids[i] - 1].push_back(i);
    }
    return groups;
}

}  //  namespace nono
