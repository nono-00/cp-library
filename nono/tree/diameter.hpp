#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/tree/is-tree.hpp"

namespace nono {

namespace internal {

//  diameter(graph)の結果
template <class T>
class DiameterResult {
  public:
    DiameterResult(T dist, std::array<int, 2> endpoints, std::vector<int> vertex_id, std::vector<int> edge_id)
        : dist_(dist),
          endpoints_(std::move(endpoints)),
          vertex_id_(std::move(vertex_id)),
          edge_id_(std::move(edge_id)) {}

    //  直径の長さ
    T dist() const {
        return dist_;
    }

    //  直径の端点
    std::array<int, 2> endpoints() const {
        return endpoints_;
    }

    //  直径のパス 頂点
    //  endpoints[0] -> ... -> ... -> endpoints[1]
    std::vector<int> vertex_id() const {
        return vertex_id_;
    }

    //  直径のパス 辺
    //  v[0] -> e[0] -> v[1] -> e[1] -> ... -> e[n - 2] -> v[n - 1]
    std::vector<int> edge_id() const {
        return edge_id_;
    }

  private:
    T dist_;
    std::array<int, 2> endpoints_;
    std::vector<int> vertex_id_;
    std::vector<int> edge_id_;
};

}  //  namespace internal

//  木の直径を求める
//  木でないと壊れる
template <class T>
internal::DiameterResult<T> diameter(const Graph<T>& graph) {
    assert(is_tree(graph));
    using Result = internal::DiameterResult<T>;
    constexpr int NONE = -1;

    std::vector<int> parent(graph.size());
    std::vector<int> parent_edge(graph.size());
    auto dfs = [&](auto&& self, int u, int p) -> std::pair<T, int> {
        parent[u] = p;
        std::pair<T, int> res = {0, u};
        for (const auto& e: graph[u]) {
            if (e.to == p) continue;
            parent_edge[e.to] = e.id;
            auto adj = self(self, e.to, u);
            adj.first += e.weight;
            res = std::max(res, adj);
        }
        return res;
    };
    std::pair<T, int> p1 = dfs(dfs, 0, NONE);
    std::pair<T, int> p2 = dfs(dfs, p1.second, NONE);
    const T dist = p2.first;
    std::array<int, 2> endpoints = {p2.second, p1.second};
    std::vector<int> vertex_id, edge_id;
    int cur = endpoints[0];
    while (true) {
        vertex_id.push_back(cur);
        if (cur == endpoints[1]) break;
        edge_id.push_back(parent_edge[cur]);
        cur = parent[cur];
    }
    assert(endpoints[0] == vertex_id.front());
    assert(endpoints[1] == vertex_id.back());
    return Result(dist, endpoints, std::move(vertex_id), std::move(edge_id));
}

}  //  namespace nono
