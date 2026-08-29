#pragma once

#include <cassert>
#include <vector>

#include "./base.hpp"

namespace nono {

///  # is bipartite(graph)
///  whether graph is bipartite
///  O(n + m)
template <class T>
bool is_bipartite(const Graph<T>& graph) {
    int n = graph.size();
    std::vector<short> color(n, -1);
    auto dfs = [&](auto&& self, int u) -> bool {
        assert(color[u] != -1);
        for (auto e: graph[u]) {
            if (color[e.to] == -1) {
                color[e.to] = color[u] ^ 1;
                if (!self(self, e.to)) return false;
            } else if (color[e.to] == color[u]) {
                return false;
            }
        }
        return true;
    };
    for (int i = 0; i < n; i++) {
        if (color[i] == -1) {
            color[i] = 0;
            if (!dfs(dfs, i)) return false;
        }
    }
    return true;
}

}  //  namespace nono
