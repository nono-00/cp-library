#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "data-structure/union-find.hpp"

namespace nono {

template <class EdgeType>
struct MinimumSpanningTree {
    EdgeType::WeightType cost;
    std::vector<EdgeType> edges;
};

//  brief:
//  - 最小全域木のコスト、使用する辺を取得する
//
//  tparam:
//  - `T`: 辺の重みの型
//
//  pre:
//  - `edges` は辺の重みによって昇順ソートされていなければならない
//
//  return:
//  - 全域木のコスト, 使用する辺の配列のペア
//
//  depend:
//  - `UnionFind`
template <class EdgeType>
MinimumSpanningTree<EdgeType> kruskal(int vertex_size, const std::vector<EdgeType>& edges) {
    using T = EdgeType::WeightType;

    T cost = 0;
    std::vector<EdgeType> used_edges;
    used_edges.reserve(vertex_size - 1);
    UnionFind uf(vertex_size);

    for (const auto& edge: edges) {
        if (not uf.same(edge.from, edge.to)) {
            uf.merge(edge.from, edge.to);
            cost += edge.weight;
            used_edges.emplace_back(edge);
        }
    }

    assert((int)used_edges.size() == vertex_size - 1);
    return {cost, used_edges};
}

}  //  namespace nono
