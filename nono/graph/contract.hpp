#pragma once

#include <iterator>
#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

template <class T>
Graph<T> contract(const Graph<T>& graph, const std::vector<std::vector<int>>& groups) {
    constexpr int NONE = -1;
    int n = graph.size();
    std::vector<int> mapping(n, NONE);
    for (int i = 0; i < std::ssize(groups); i++) {
        const auto& group = groups[i];
        for (int j: group) {
            mapping[j] = i;
        }
    }
    std::vector<bool> used(graph.edge_size());
    std::vector<EdgeBase<T>> edges;
    for (int u = 0; u < n; u++) {
        if (mapping[u] == NONE) continue;
        for (const auto& e: graph[u]) {
            if (used[e.id]) continue;
            if (mapping[e.to] == NONE) continue;
            if (mapping[u] == mapping[e.to]) continue;
            used[e.id] = true;
            edges.emplace_back(mapping[u], mapping[e.to], e.weight);
        }
    }
    if (graph.is_directed()) {
        return to_directed_graph(groups.size(), edges);
    } else {
        return to_undirected_graph(groups.size(), edges);
    }
}

}  //  namespace nono
