#pragma once

#include <algorithm>
#include <vector>

#include "nono/tree/internal-tree-concepts.hpp"

namespace nono {

namespace tree {

//  brief:
//  - 木かどうか判定する
template <internal::Tree TreeType>
bool is_tree(const TreeType& graph) {
    constexpr int NONE = -1;
    int n = graph.size();
    std::vector<bool> used(n);
    auto dfs = [&](auto self, int u, int p) -> bool {
        used[u] = true;
        for (const auto& e: graph[u]) {
            if (e.to == p) continue;
            if (used[e.to]) return false;
            if (!self(self, e.to, u)) return false;
        }
        return true;
    };
    if (!dfs(dfs, graph.root(), NONE)) return false;
    return std::ranges::all_of(used, [](bool f) { return f; });
}

}  //  namespace tree

}  //  namespace nono
