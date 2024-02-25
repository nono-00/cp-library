#pragma once

#include <limits>
#include <queue>
#include <vector>

#include "nono/graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - bfs
//
//  complexity:
//  - O(V + E)
//
//  return:
//  - 到達不可能な頂点は `-INF`
template <internal::WeightedGraph GraphType>
auto bfs(const GraphType& graph, int source) {
    using T = GraphType::EdgeType::WeightType;
    constexpr int INF = std::numeric_limits<T>::min();

    std::vector<T> dist(graph.size(), INF);
    dist[source] = 0;
    std::queue<int> que;
    que.push(source);

    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (const auto& edge: graph[u]) {
            if (dist[edge.to] == INF) {
                dist[edge.to] = dist[u] + edge.weight;
                que.push(edge.to);
            }
        }
    }

    return dist;
}

}  //  namespace nono
