#pragma once

#include <limits>
#include <queue>
#include <vector>

namespace nono {

//  brief:
//  - bfs
//
//  complexity:
//  - O(V + E)
//
//  return:
//  - 到達不可能な頂点は `INF`
template <class GraphType>
auto bfs(const GraphType& graph, int source) {
    using T = typename GraphType::WeightType;
    constexpr T INF = std::numeric_limits<T>::max();

    std::vector<T> dist(graph.size(), INF);
    dist[source] = static_cast<T>(0);
    std::queue<int> vertex;
    vertex.push(source);

    while (!vertex.empty()) {
        int u = vertex.front();
        vertex.pop();
        for (const auto& edge: graph[u]) {
            if (dist[edge.to] == INF) {
                dist[edge.to] = dist[u] + edge.weight;
                vertex.push(edge.to);
            }
        }
    }

    return dist;
}

}  //  namespace nono
