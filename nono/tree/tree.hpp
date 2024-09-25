#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/tree/centroids.hpp"
#include "nono/tree/diameter.hpp"
#include "nono/tree/is-tree.hpp"

namespace nono {

template <class T>
class Tree: public Graph<T> {
  public:
    Tree() {}
    Tree(int n, const std::vector<EdgeBase<T>>& edges, int root = 0)
        : Graph<T>(to_undirected_graph(n, edges)),
          n_(n),
          root_(root),
          weighted_depth_(n_),
          weighted_height_(n_),
          depth_(n_),
          height_(n_),
          in_(n_),
          out_(n_),
          head_(n_, root_),
          parent_(n_, -1) {
        assert(is_tree(*this));
        set_heavy_edge(root_, -1);
        dfs(root_, -1, 0);
    }
    //  least common ancester
    int lca(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        while (head_[lhs] != head_[rhs]) {
            if (in_[lhs] > in_[rhs]) std::swap(lhs, rhs);
            rhs = parent_[head_[rhs]];
        }
        return (depth_[lhs] < depth_[rhs] ? lhs : rhs);
    }

    //  lhsとrhsの距離
    int dist(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        return depth_[lhs] + depth_[rhs] - 2 * depth_[lca(lhs, rhs)];
    }

    //  lhsとrhsの距離
    T weighted_dist(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        return weighted_depth_[lhs] + weighted_depth_[rhs] - 2 * weighted_depth_[lca(lhs, rhs)];
    }

    //  level ancester
    //  根方向にk回移動
    //  移動できないと壊れる
    int jump(int x, int k) {
        assert(0 <= x && x < n_);
        assert(0 <= k && k <= depth_[x]);
        while (k > depth_[x] - depth_[head_[x]]) {
            k -= depth_[x] - depth_[head_[x]] + 1;
            x = parent_[head_[x]];
        }
        assert(k >= 0);
        return tour_[in_[head_[x]] + depth_[x] - depth_[head_[x]] - k];
    }

    //  jump on tree
    //  移動できないと壊れる
    int jump(int from, int to, int k) {
        assert(0 <= from && from < n_);
        assert(0 <= to && to < n_);
        int mid = lca(from, to);
        assert(0 <= k && k <= depth_[from] + depth_[to] - 2 * depth_[mid]);
        if (k <= depth_[from] - depth_[mid]) {
            return jump(from, k);
        } else {
            return jump(to, depth_[from] + depth_[to] - 2 * depth_[mid] - k);
        }
    }

    //  xの深さ
    int depth(int x) {
        assert(0 <= x && x < n_);
        return depth_[x];
    }

    //  xの深さ
    T weighted_depth(int x) {
        assert(0 <= x && x < n_);
        return weighted_depth_[x];
    }

    //  xの高さ
    int height(int x) {
        assert(0 <= x && x < n_);
        return height_[x];
    }

    //  xの高さ
    T weighted_height(int x) {
        assert(0 <= x && x < n_);
        return weighted_height_[x];
    }

    //  x含む部分木内の頂点数
    int subtree(int x) {
        assert(0 <= x && x < n_);
        return out_[x] - in_[x];
    }

    //  直径
    auto diameter() {
        return nono::diameter(*this);
    }

    //  重心列挙
    auto centroids() {
        return nono::centroids(*this);
    }

  protected:
    int n_;
    int root_;
    std::vector<T> weighted_depth_;
    std::vector<T> weighted_height_;
    std::vector<int> depth_;
    std::vector<int> height_;
    std::vector<int> in_;
    std::vector<int> out_;
    std::vector<int> head_;
    std::vector<int> parent_;
    std::vector<int> tour_;

  private:
    int set_heavy_edge(int u, int p) {
        int size_sum = 1;
        int max_size = 0;
        int max_size_i = 0;
        auto&& adj = (*this)[u];
        for (int i = 0; i < std::ssize(adj); i++) {
            const auto& e = adj[i];
            if (e.to == p) continue;
            int size = set_heavy_edge(e.to, u);
            size_sum += size;
            if (max_size < size) {
                max_size = size;
                max_size_i = i;
            }
        }
        if (max_size_i != 0) {
            std::swap(adj[0], adj[max_size_i]);
        }
        return size_sum;
    }

    int dfs(int u, int p, int now) {
        tour_.push_back(u);
        parent_[u] = p;
        in_[u] = now++;
        bool heavy = true;
        for (const auto& e: (*this)[u]) {
            if (e.to == p) continue;
            head_[e.to] = (heavy ? head_[u] : e.to);
            weighted_depth_[e.to] = weighted_depth_[u] + e.weight;
            depth_[e.to] = depth_[u] + 1;
            heavy = false;
            now = dfs(e.to, u, now);
            weighted_height_[u] = std::max(weighted_height_[u], weighted_height_[e.to] + e.weight);
            height_[u] = std::max(height_[u], height_[e.to] + 1);
        }
        out_[u] = now;
        return now;
    }
};

}  //  namespace nono
