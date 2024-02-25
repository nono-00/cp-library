#pragma once

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

#include "nono/graph/bfs.hpp"
#include "nono/graph/internal-graph-concepts.hpp"
#include "nono/tree/internal-tree-concepts.hpp"

namespace nono {

template <internal::Edge EdgeType>
struct DiameterInfo {
    EdgeType::WeightType dist;
    std::array<int, 2> endpoints;
    std::vector<EdgeType> path;
};

//  brief:
//  - 木の直径を求める
//
//  return:
//  - 木の直径の距離、両端点、パス
//  - パスはendpoints[0]からendpoints[1]へのパス
//
//  note:
//  - 重み付きグラフに対してのみ動く
//  - 重みなしならedge.weight = 1としておく
template <internal::WeightedTree TreeType>
auto diameter(const TreeType& tree) {
    using T = TreeType::EdgeType::WeightType;
    constexpr int NONE = -1;

    DiameterInfo<typename TreeType::EdgeType> info;
    auto dfs = [&](auto self, int u, int p) -> std::pair<T, int> {
        std::pair<T, int> res = {0, u};
        for (const auto& e: tree[u]) {
            if (e.to == p) continue;
            auto temp = self(self, e.to, u);
            temp.first += e.weight;
            res = std::max(res, temp);
        }
        return res;
    };
    auto p1 = dfs(dfs, tree.root(), NONE);
    auto p2 = dfs(dfs, p1.second, NONE);
    info.dist = p2.first;
    info.endpoints = {p1.second, p2.second};
    auto get_path = [&](auto self, int u, int p, int g) -> bool {
        if (u == g) return true;
        for (const auto& e: tree[u]) {
            if (e.to == p) continue;
            if (self(self, e.to, u, g)) {
                info.path.push_back(e);
                return true;
            }
        }
        return false;
    };
    get_path(get_path, info.endpoints[0], NONE, info.endpoints[1]);
    std::ranges::reverse(info.path);
    return info;
}

}  //  namespace nono
