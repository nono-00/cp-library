#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include "../ds/union-find.hpp"
#include "./base.hpp"

namespace nono {

namespace internal {

///  # kruskal result
template <class T>
class KruskalResult {
  public:
    KruskalResult(T cost, std::vector<int> edge_id): cost_(cost), edge_id_(std::move(edge_id)) {}

    ///  # min cost
    ///  cost of min spanning tree
    T min_cost() const {
        return cost_;
    }

    ///  # edge id
    ///  edge idof min spanning tree
    std::vector<int> edge_id() const {
        return edge_id_;
    }

  private:
    T cost_;
    std::vector<int> edge_id_;
};

}  //  namespace internal

///  brief : 最小全域木を求める.

///  # kruskal(n, edges)
///  最小全域木のコスト、使用する辺を取得する
///  連結グラフでないと壊れる
///  辺はソートされていなくても良い
///  O(m log m)
template <class T>
internal::KruskalResult<T> kruskal(int n, const std::vector<EdgeBase<T>>& edges) {
    using Result = internal::KruskalResult<T>;

    std::vector<int> edge_id(edges.size());
    std::iota(edge_id.begin(), edge_id.end(), 0);
    std::ranges::sort(edge_id, [&edges](int lhs, int rhs) { return edges[lhs].weight < edges[rhs].weight; });
    T cost = 0;
    std::vector<int> used_edge_id;
    used_edge_id.reserve(n - 1);
    UnionFind uf(n);

    for (int i: edge_id) {
        const auto& e = edges[i];
        if (!uf.same(e.from, e.to)) {
            uf.merge(e.from, e.to);
            cost += e.weight;
            used_edge_id.push_back(i);
        }
    }

    assert(std::ssize(used_edge_id) == n - 1);
    return Result(cost, used_edge_id);
}

}  //  namespace nono
