#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "./base.hpp"

namespace nono {

///  # two edge connected components
///  二辺連結成分分解する.
///  連結非連結, 単純非単純を問わない.
///  O(n + m)
template <class T>
std::vector<std::vector<int>> two_edge_connected_components(const Graph<T>& graph) {
    assert(graph.is_undirected());
    constexpr int NONE = -1;

    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> lowlink(n);
    std::vector<int> stack;
    std::vector<std::vector<int>> groups;
    int now = 0;

    auto dfs = [&](const auto& self, int u, int edge_id) -> void {
        order[u] = now++;
        lowlink[u] = order[u];
        for (const auto& edge: graph[u]) {
            if (edge.id == edge_id) continue;
            if (order[edge.to] == NONE) {
                self(self, edge.to, edge.id);
                lowlink[u] = std::min(lowlink[u], lowlink[edge.to]);
                if (order[u] < lowlink[edge.to]) {
                    std::vector<int> group;
                    while (!stack.empty()) {
                        int v = stack.back();
                        if (order[u] >= lowlink[v]) break;
                        stack.pop_back();
                        group.push_back(v);
                    }
                    groups.push_back(group);
                }
            } else {
                lowlink[u] = std::min(lowlink[u], order[edge.to]);
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
