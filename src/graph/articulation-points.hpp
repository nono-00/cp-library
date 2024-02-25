#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "graph/base.hpp"

namespace nono {

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
    for (int i = 0; i < n; i++) {
        if (parent[i] == NONE) {
            if (children[i] >= 2) result.push_back(i);
        } else if (lowlink[i] >= order[i]) {
            result.push_back(i);
        }
    }
    return result;
}

}  //  namespace nono
