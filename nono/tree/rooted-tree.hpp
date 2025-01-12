#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

///  # to rooted tree(n, edges, root)
///  rootから有向木に
///  O(n)
template <class T>
Graph<T> to_rooted_tree(int n, std::vector<EdgeBase<T>> edges, int root = 0) {
    auto graph = to_undirected_graph(n, edges);
    assert(is_tree(graph));
    auto dfs = [&](auto&& self, int u, int p) -> void {
        for (auto e: graph[u]) {
            if (e.to == p) continue;
            if (edges[e.id].to != e.to) {
                std::swap(edges[e.id].from, edges[e.id].to);
            }
            self(self, e.to, u);
        }
    };
    dfs(dfs, root, -1);
    return to_directed_graph(n, edges);
}

}  //  namespace nono
