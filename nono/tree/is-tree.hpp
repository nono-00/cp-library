#pragma once

#include <algorithm>
#include <vector>

#include "../graph/base.hpp"

namespace nono {

///  brief  : 木かどうか判定する
template <class T>
bool is_tree(const Graph<T>& graph) {
    if (graph.is_directed()) return false;
    constexpr int NONE = -1;
    int n = graph.size();
    std::vector<int> used(n);
    auto dfs = [&](auto self, int u, int eid) -> bool {
        used[u] = 1;
        for (const auto& e: graph[u]) {
            if (e.id == eid) continue;
            if (used[e.to]) return false;
            if (!self(self, e.to, e.id)) return false;
        }
        return true;
    };
    if (!dfs(dfs, 0, NONE)) return false;
    return std::ranges::all_of(used, [](int f) { return f == 1; });
}

}  //  namespace nono
