#pragma once

#include <limits>
#include <queue>
#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

//  幅優先探索
//
//  到達できない頂点の値はstd::numeric_limits<T>::min()
template <class T>
std::vector<T> bfs(const Graph<T>& graph, int source) {
    constexpr T NONE = std::numeric_limits<T>::min();

    std::vector<T> dist(graph.size(), NONE);
    dist[source] = 0;
    std::queue<int> que;
    que.push(source);

    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (const auto& e: graph[u]) {
            if (dist[e.to] == NONE) {
                dist[e.to] = dist[u] + e.weight;
                que.push(e.to);
            }
        }
    }

    return dist;
}

}  //  namespace nono
