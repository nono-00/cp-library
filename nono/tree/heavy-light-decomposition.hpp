#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

//  # HeavyLightDecomposition
//  HL分解。頂点0を根として、パスや部分木を「列の上の区間」に変換する
//  返す区間はすべて半開区間 [l, r) で、添字は頂点の行きがけ順
//  辺は、深いほうの端点の添字で表す
template <class T>
class HeavyLightDecomposition {
    static constexpr int NONE = -1;
    using Point = int;
    using Interval = std::pair<int, int>;
    using Intervals = std::vector<std::pair<int, int>>;

  public:
    //  # HeavyLightDecomposition(graph)
    //  graphが木でないと壊れる
    //  O(n)
    HeavyLightDecomposition(Graph<T> graph)
        : graph_(std::move(graph)),
          n_(graph_.size()),
          depth_(n_),
          parent_(n_, NONE),
          head_(n_),
          in_(n_),
          out_(n_) {
        assert(is_tree(graph_));
        set_heavy_edge(0, NONE);
        dfs(0, NONE, 0);
    }

    //  # lowest_common_ancester(u, v)
    //  最小共通祖先(LCA)
    //  O(log n)
    int lowest_common_ancester(int u, int v) {
        assert(0 <= u && u < n_);
        assert(0 <= v && v < n_);
        while (head_[u] != head_[v]) {
            if (in_[u] > in_[v]) std::swap(u, v);
            v = parent_[head_[v]];
        }
        return (depth_[u] < depth_[v] ? u : v);
    }

    //  # distance(u, v)
    //  uからvへのパスの辺数
    //  O(log n)
    int distance(int u, int v) {
        return depth_[u] + depth_[v] - 2 * depth_[lowest_common_ancester(u, v)];
    }

    //  # vertex(v)
    //  頂点vに対応する添字
    Point vertex(int v) {
        return in_[v];
    }

    //  # edge(u, v)
    //  辺(u, v)に対応する添字
    //  uとvが隣接していないと壊れる
    Point edge(int u, int v) {
        if (depth_[u] > depth_[v]) std::swap(u, v);
        assert(parent_[v] == u);
        return in_[v];
    }

    //  # vertices_for_subtree(u)
    //  uの部分木の頂点に対応する区間
    Interval vertices_for_subtree(int u) {
        return subtree(u, true);
    }

    //  # edges_for_subtree(u)
    //  uの部分木の辺に対応する区間
    Interval edges_for_subtree(int u) {
        return subtree(u, false);
    }

    //  # vertices_for_path(u, v)
    //  uからvへのパス上の頂点に対応する区間の列。区間の数は O(log n)
    //  区間の並びはパスを辿る順とは一致しない。空の区間を含むことがある
    Intervals vertices_for_path(int u, int v) {
        return path(u, v, true);
    }

    //  # edges_for_path(u, v)
    //  uからvへのパス上の辺に対応する区間の列。区間の数は O(log n)
    //  区間の並びはパスを辿る順とは一致しない。空の区間を含むことがある
    Intervals edges_for_path(int u, int v) {
        return path(u, v, false);
    }

  private:
    int set_heavy_edge(int u, int p) {
        int size_sum = 1;
        int max_size = 0;
        int max_size_i = 0;
        auto&& adj = graph_[u];
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
        parent_[u] = p;
        in_[u] = now++;
        bool heavy = true;
        for (const auto& e: graph_[u]) {
            if (e.to == p) continue;
            head_[e.to] = (heavy ? head_[u] : e.to);
            depth_[e.to] = depth_[u] + 1;
            heavy = false;
            now = dfs(e.to, u, now);
        }
        out_[u] = now;
        return now;
    }

    Interval subtree(int u, bool vertex) {
        return vertex ? Interval(in_[u], out_[u]) : Interval(in_[u] + 1, out_[u]);
    }

    Intervals path(int u, int v, bool vertex) {
        int l = lowest_common_ancester(u, v);
        Intervals result;
        for (auto w: {u, v}) {
            while (head_[w] != head_[l]) {
                result.emplace_back(in_[head_[w]], in_[w] + 1);
                w = parent_[head_[w]];
            }
            result.emplace_back(in_[l] + 1, in_[w] + 1);
        }
        if (vertex) result.emplace_back(in_[l], in_[l] + 1);
        return result;
    }

    Graph<T> graph_;
    int n_;
    std::vector<int> depth_;
    std::vector<int> parent_;
    std::vector<int> head_;
    std::vector<int> in_;
    std::vector<int> out_;
};

}  //  namespace nono
