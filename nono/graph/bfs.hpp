#pragma once

#include <optional>
#include <queue>
#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

///  brief : 幅優先探索. 使ったことがない.
template <class T>
std::vector<std::optional<T>> bfs(const Graph<T>& graph, int source) {
    std::vector<std::optional<T>> dist(graph.size());
    dist[source] = 0;
    std::queue<int> que;
    que.push(source);

    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (const auto& e: graph[u]) {
            if (!dist[e.to]) {
                dist[e.to] = *dist[u] + e.weight;
                que.push(e.to);
            }
        }
    }

    return dist;
}

}  //  namespace nono
