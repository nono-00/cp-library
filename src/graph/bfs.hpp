#pragma once

#include <limits>
#include <queue>
#include <vector>

#include "graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - bfs
//
//  complexity:
//  - O(V + E)
//
//  return:
//  - 到達不可能な頂点は `-INF`
template <internal::Graph GraphType>
std::vector<int> bfs(const GraphType& graph, int source) {
    constexpr int INF = std::numeric_limits<int>::min();

    std::vector<int> dist(graph.size(), INF);
    dist[source] = 0;
    std::queue<int> que;
    que.push(source);

    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (const auto& edge: graph[u]) {
            if (dist[edge.to] == INF) {
                dist[edge.to] = dist[u] + 1;
                que.push(edge.to);
            }
        }
    }

    return dist;
}

}  //  namespace nono
