#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>
#include <vector>

#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

namespace diameter_internal {

//  # DiameterResult
//  diameterの結果
template <class T>
class DiameterResult {
  public:
    DiameterResult(T dist, std::array<int, 2> endpoints, std::vector<int> vertex_id, std::vector<int> edge_id)
        : dist_(dist),
          endpoints_(std::move(endpoints)),
          vertex_id_(std::move(vertex_id)),
          edge_id_(std::move(edge_id)) {}

    //  # dist()
    //  直径の長さ(辺の重みの総和)
    T dist() const {
        return dist_;
    }

    //  # endpoints()
    //  直径の両端の頂点
    std::array<int, 2> endpoints() const {
        return endpoints_;
    }

    //  # vertex_id()
    //  直径上の頂点の列。endpoints()[0] から endpoints()[1] へ向かう順に並ぶ
    std::vector<int> vertex_id() const {
        return vertex_id_;
    }

    //  # edge_id()
    //  直径上の辺の番号の列。vertex_id()と同じ向きに並ぶ
    //  v[0] -> e[0] -> v[1] -> e[1] -> ... -> e[k - 2] -> v[k - 1]
    std::vector<int> edge_id() const {
        return edge_id_;
    }

  private:
    T dist_;
    std::array<int, 2> endpoints_;
    std::vector<int> vertex_id_;
    std::vector<int> edge_id_;
};

}  //  namespace diameter_internal

//  # diameter(graph)
//  木の直径(最も遠い2頂点を結ぶパス)を求める。辺の重みを距離として使う
//  木でないと壊れる。負の重みの辺があると壊れる(直径にならない)
//  O(n)
template <class T>
diameter_internal::DiameterResult<T> diameter(const Graph<T>& graph) {
    assert(is_tree(graph));
    using Result = diameter_internal::DiameterResult<T>;
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
