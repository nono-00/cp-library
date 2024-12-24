#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "./base.hpp"

namespace nono {

///  brief : 橋列挙

//  無向グラフでないと壊れる
//  非連結、非単純グラフでも動くはず
//
//  橋: その辺を取り除くとグラフが非連結となるような辺.
template <class T>
std::vector<int> bridges(const Graph<T>& graph) {
    assert(graph.is_undirected());
    constexpr int NONE = -1;
    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> result;

    int now = 0;
    auto dfs = [&](const auto& self, int u, int edge_id) -> int {
        int lowlink = order[u] = now++;
        for (const auto& e: graph[u]) {
            if (e.id == edge_id) continue;
            if (order[e.to] == NONE) {
                auto adj_low = self(self, e.to, e.id);
                lowlink = std::min(adj_low, lowlink);
                if (order[u] < adj_low) {
                    result.push_back(e.id);
                }
            } else {
                lowlink = std::min(lowlink, order[e.to]);
            }
        }
        return lowlink;
    };

    for (int i = 0; i < n; i++) {
        if (order[i] == NONE) {
            dfs(dfs, i, NONE);
        }
    }

    return result;
}

}  //  namespace nono
