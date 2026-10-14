#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "../graph/base.hpp"
#include "./centroids.hpp"
#include "./diameter.hpp"
#include "./is-tree.hpp"

namespace nono {

//  # Tree
//  根付き木。graph + lca + la + jump + dist + depth + height + centroid + diameter ...
//  Weighted版はWeightedTreeへ分離（辺重みTにoperator+を要求しないため）
//  各メンバ関数に渡す頂点が [0, n) でないと壊れる
template <class T>
class Tree: public Graph<T> {
  public:
    Tree() {}

    //  # Tree(n, edges, root)
    //  頂点数がn、辺がedges、根がrootの木を作る
    //  edgesが木でないと壊れる
    //  O(n)
    Tree(int n, const std::vector<EdgeBase<T>>& edges, int root = 0)
        : Graph<T>(to_undirected_graph(n, edges)),
          n_(n),
          root_(root),
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

    //  # lca(lhs, rhs)
    //  最小共通祖先(LCA)
    //  O(log n)
    int lca(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        while (head_[lhs] != head_[rhs]) {
            if (in_[lhs] > in_[rhs]) std::swap(lhs, rhs);
            rhs = parent_[head_[rhs]];
        }
        return (depth_[lhs] < depth_[rhs] ? lhs : rhs);
    }

    //  # dist(lhs, rhs)
    //  lhsからrhsへのパスの辺数(重みは見ない)
    //  O(log n)
    int dist(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        return depth_[lhs] + depth_[rhs] - 2 * depth_[lca(lhs, rhs)];
    }

    //  # jump(x, k)
    //  level ancestor。xから根の方向へk回移動した頂点
    //  0 <= k <= depth(x) でないと壊れる
    //  O(log n)
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

    //  # jump(from, to, k)
    //  fromからtoへ向かうパス上を、k回移動した頂点
    //  0 <= k <= dist(from, to) でないと壊れる
    //  O(log n)
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

    //  # depth(x)
    //  根からxまでの辺数(重みは見ない)
    //  O(1)
    int depth(int x) {
        assert(0 <= x && x < n_);
        return depth_[x];
    }

    //  # height(x)
    //  xの部分木の高さ。xから、部分木内で最も遠い頂点までの辺数
    //  O(1)
    int height(int x) {
        assert(0 <= x && x < n_);
        return height_[x];
    }

    //  # subtree(x)
    //  xの部分木の頂点数(x自身を含む)
    //  O(1)
    int subtree(int x) {
        assert(0 <= x && x < n_);
        return out_[x] - in_[x];
    }

    //  # diameter()
    //  木の直径。nono::diameter(graph) の結果を返す
    //  O(n)
    auto diameter() {
        return nono::diameter(*this);
    }

    //  # centroids()
    //  重心分解。nono::centroids(graph) の結果を返す
    //  O(n log n)
    auto centroids() {
        return nono::centroids(*this);
    }

  protected:
    int n_;
    int root_;
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
            depth_[e.to] = depth_[u] + 1;
            heavy = false;
            now = dfs(e.to, u, now);
            height_[u] = std::max(height_[u], height_[e.to] + 1);
        }
        out_[u] = now;
        return now;
    }
};

}  //  namespace nono
