#pragma once

#include <cassert>
#include <vector>

#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

/// brief : 木の重心を行きがけ順で列挙する.
/// TODO : verify
//
//  良く書くので...
//
//  木ではないと壊れる
template <class T>
std::vector<int> centroids(const Graph<T>& graph) {
    assert(is_tree(graph));
    int n = graph.size();
    std::vector<int> subtree(n);
    std::vector<bool> removed(n);
    std::vector<int> result;
    result.reserve(n);

    auto calc_subtree = [&](auto&& self, int u, int p) -> int {
        subtree[u] = 1;
        for (const auto& e: graph[u]) {
            if (e.to == p || removed[e.to]) continue;
            subtree[u] += self(self, e.to, u);
        }
        return subtree[u];
    };

    std::vector<int> cur;
    cur.push_back(0);
    while (!cur.empty()) {
        std::vector<int> next;
        for (auto root: cur) {
            calc_subtree(calc_subtree, root, root);
            int centroid = root;
            bool flag = true;
            while (flag) {
                flag = false;
                for (const auto& e: graph[centroid]) {
                    if (removed[e.to] || subtree[e.to] > subtree[centroid]) continue;
                    if (subtree[root] <= 2 * subtree[e.to]) {
                        centroid = e.to;
                        flag = true;
                    }
                }
            }
            result.push_back(centroid);
            removed[centroid] = true;
            for (const auto& e: graph[centroid]) {
                if (removed[e.to]) continue;
                next.push_back(e.to);
            }
        }
        cur = std::move(next);
    }
    return result;
}

}  //  namespace nono
