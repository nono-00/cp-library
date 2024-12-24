#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "./base.hpp"

namespace nono {

///  brief : 関節点列挙

//  無向グラフでないと壊れる
//  頂点番号の昇順で返す
//  非連結、非単純グラフでも動くはず
template <class T>
std::vector<int> articulation_points(const Graph<T>& graph) {
    assert(graph.is_undirected());
    constexpr int NONE = -1;
    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> lowlink(n, NONE);
    std::vector<int> parent(n, NONE);
    std::vector<int> children(n);

    int now = 0;
    auto dfs = [&](const auto& dfs, int u, int edge_id) -> void {
        order[u] = lowlink[u] = now++;
        for (const auto& e: graph[u]) {
            if (e.id == edge_id) continue;
            if (order[e.to] == NONE) {
                parent[e.to] = u;
                children[u]++;
                dfs(dfs, e.to, e.id);
                lowlink[u] = std::min(lowlink[u], lowlink[e.to]);
            } else {
                lowlink[u] = std::min(lowlink[u], order[e.to]);
            }
        }
    };
    for (int i = 0; i < n; i++) {
        if (order[i] == NONE) {
            dfs(dfs, i, NONE);
        }
    }
    std::vector<int> result;
    for (int u = 0; u < n; u++) {
        if (parent[u] == NONE) {
            if (children[u] >= 2) result.push_back(u);
        } else {
            for (const auto& e: graph[u]) {
                if (parent[e.to] != u) continue;
                if (order[u] <= lowlink[e.to]) {
                    result.push_back(u);
                    break;
                }
            }
        }
    }
    return result;
}

}  //  namespace nono
