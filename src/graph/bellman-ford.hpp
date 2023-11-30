#pragma once

#include <limits>
#include <vector>

namespace nono {

//  brief:
//  - 負辺ありの単一始点最短経路問題を解く
//
//  complexity:
//  - O(VE)
//
//  return:
//  - 最短距離が確定する頂点は最短距離
//  - 最短距離が確定しない頂点は `-INF`
//  - 辿り着けない頂点は `INF`
//
//  todo:
//  - 負閉路に属する頂点の取得
template <class GraphType>
auto bellman_ford(const GraphType& graph, int source) {
    using T = typename GraphType::WeightType;
    constexpr T INF = std::numeric_limits<T>::max();

    int n = graph.size();
    std::vector<T> dist(graph.size(), INF);
    dist[source] = static_cast<T>(0);

    for (int i = 0; i < n; i++) {
        bool update = false;
        for (int u = 0; u < n; u++) {
            if (dist[u] == INF) continue;
            for (const auto& e: graph[u]) {
                if (dist[e.to] > dist[u] + e.weight) {
                    update = true;
                    dist[e.to] = dist[u] + e.weight;
                }
            }
        }

        if (not update) break;
    }

    for (int i = 0; i < graph.size(); i++) {
        bool update = false;
        for (int u = 0; u < graph.size(); u++) {
            if (dist[u] == INF) continue;
            for (const auto& e: graph[u]) {
                if (dist[u] == -INF) {
                    update = true;
                    dist[e.to] = -INF;
                } else if (dist[e.to] > dist[u] + e.weight) {
                    update = true;
                    dist[e.to] = -INF;
                }
            }
        }

        if (not update) break;
    }

    return dist;
}

}  //  namespace nono
