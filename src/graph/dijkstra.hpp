#pragma once

#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - 負辺無し単一始点最短経路問題を解く
//
//  complexity:
//  - O((V + E)logV)
//
//  tparam:
//  - `GraphType`: グラフ型
//
//  return:
//  - `source` からたどり着くことができるのならば, `source` からの最短経路
//  - そうでなければ `numeric_limit<T>::max()`
template <internal::WeightedGraph GraphType>
auto dijkstra(const GraphType& graph, int source) {
    using T = typename GraphType::EdgeType::WeightType;
    constexpr T INF = std::numeric_limits<T>::max();

    std::vector<T> dist(graph.size(), INF);
    dist[source] = static_cast<T>(0);
    std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>, std::greater<std::pair<T, int>>> pque;
    pque.emplace(dist[source], source);

    while (!pque.empty()) {
        auto [d, u] = pque.top();
        pque.pop();
        if (dist[u] < d) continue;
        for (const auto& e: graph[u]) {
            if (dist[e.to] > dist[u] + e.weight) {
                dist[e.to] = dist[u] + e.weight;
                pque.emplace(dist[e.to], e.to);
            }
        }
    }

    return dist;
}

}  //  namespace nono
