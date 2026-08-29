#pragma once

#include <algorithm>
#include <vector>

#include "../graph/base.hpp"
#include "./tree.hpp"

namespace nono {

///  # WeightedTree
///  Tree + weighted_depth + weighted_height + weighted_dist
///  辺重みTにoperator+/operator<を要求する
template <class T>
class WeightedTree: public Tree<T> {
  public:
    WeightedTree() {}
    WeightedTree(int n, const std::vector<EdgeBase<T>>& edges, int root = 0)
        : Tree<T>(n, edges, root), weighted_depth_(n), weighted_height_(n) {
        dfs(root, -1);
    }

    ///  # weighted_dist(lhs, rhs)
    ///  distance from lhs to rhs (with weight)
    ///  sum of edges weight of path
    ///  O(log n)
    T weighted_dist(int lhs, int rhs) {
        return weighted_depth_[lhs] + weighted_depth_[rhs] - 2 * weighted_depth_[this->lca(lhs, rhs)];
    }

    ///  # weighted_depth(x)
    ///  distance from x to root (with weight)
    ///  sum of edges weight of path
    ///  O(1)
    T weighted_depth(int x) const {
        return weighted_depth_[x];
    }

    ///  # weighted_height(x)
    ///  height of vertex x (with weight)
    ///  O(1)
    T weighted_height(int x) const {
        return weighted_height_[x];
    }

  private:
    std::vector<T> weighted_depth_;
    std::vector<T> weighted_height_;

    void dfs(int u, int p) {
        for (const auto& e: (*this)[u]) {
            if (e.to == p) continue;
            weighted_depth_[e.to] = weighted_depth_[u] + e.weight;
            dfs(e.to, u);
            weighted_height_[u] = std::max(weighted_height_[u], weighted_height_[e.to] + e.weight);
        }
    }
};

}  //  namespace nono
