#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "nono/tree/internal-tree-concepts.hpp"

namespace nono {

//  brief:
//  - 重軽分解
//
//  details:
//  - 木をパスに分解することによって列に変換する
//  - 任意のパスがO(log N)の直線パスに分解できるので,
//  - FenwickTree, SegmentTreeなどを用いることで、
//  - 木のパスクエリにO((logN) ^ 2)で回答可能.
template <internal::Tree TreeType>
class HeavyLightDecomposition {
    static constexpr int NONE = -1;

  public:
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(N)
    HeavyLightDecomposition(const TreeType& graph, int root = 0)
        : graph_(graph),
          parent_(graph_.size(), NONE),
          head_(graph_.size(), NONE),
          size_(graph_.size(), NONE),
          depth_(graph_.size(), NONE),
          discover_(graph_.size(), NONE),
          finish_(graph_.size(), NONE) {
        dfs_size(root);
        int now = 0;
        head_[root] = root;
        depth_[root] = 0;
        dfs_hld(now, root);
    }

    //  brief:
    //  - 辺(u, v)に対応するindexを取得
    //
    //  complexity:
    //  - O(1)
    int edge(int u, int v) {
        if (depth_[u] > depth_[v]) {
            return discover_[u];
        } else {
            return discover_[v];
        }
    }

    //  brief:
    //  - 頂点uに対応するindexを取得
    int vertex(int u) {
        return discover_[u];
    }

    //  brief:
    //  - 頂点u, vの最近共通祖先を取得する
    //
    //  complexity:
    //  - O(log N)
    int lowest_common_ancester(int u, int v) {
        while (head_[u] != head_[v]) {
            if (depth_[head_[u]] > depth_[head_[v]]) std::swap(u, v);
            v = parent_[head_[v]];
        }
        return (depth_[u] < depth_[v] ? u : v);
    }

    //  brief:
    //  - 頂点uを根とする部分木の頂点集合に対応する
    //  - 区間を取得する
    std::pair<int, int> subtree(int u) {
        return {discover_[u], finish_[u]};
    }

    //  brief:
    //  - (u, v)パスを複数のパスに分解する
    //
    //  complexity:
    //  - O(log N)
    //
    //  note:
    //  - パスの辺に対応する区間が得られる //  - 頂点を得たい場合はvertec(lca(u, v))も加える
    std::vector<std::pair<int, int>> path(int u, int v) {
        std::vector<std::pair<int, int>> result;
        while (head_[u] != head_[v]) {
            if (depth_[head_[u]] > depth_[head_[v]]) std::swap(u, v);
            assert(discover_[head_[v]] < discover_[v] + 1);
            result.emplace_back(discover_[head_[v]], discover_[v] + 1);
            assert(head_[parent_[head_[v]]] != head_[v]);
            v = parent_[head_[v]];
            assert(v != NONE);
        }
        if (u != v) {
            if (depth_[u] > depth_[v]) std::swap(u, v);
            assert(depth_[v] - depth_[u] == discover_[v] - discover_[u]);
            result.emplace_back(discover_[u] + 1, discover_[v] + 1);
        }
        return result;
    }

  private:
    void dfs_size(int u, int p = NONE) {
        size_[u] = 1;
        int max_size = NONE;
        int max_child = NONE;
        for (int i = 0; const auto& e: graph_[u]) {
            if (e.to != p) {
                dfs_size(e.to, u);
                size_[u] += size_[e.to];
                if (max_size < size_[e.to]) {
                    max_size = size_[e.to];
                    max_child = i;
                }
            }
            i++;
        }
        if (max_child != NONE && max_child != 0) {
            std::swap(graph_[u][0], graph_[u][max_child]);
        }
    }

    void dfs_hld(int& now, int u, int p = NONE) {
        assert(head_[u] != NONE);
        assert(depth_[u] != NONE);
        discover_[u] = now++;
        for (int i = 0; const auto& e: graph_[u]) {
            if (e.to != p) {
                parent_[e.to] = u;
                head_[e.to] = (i == 0 ? head_[u] : e.to);
                depth_[e.to] = depth_[u] + 1;
                dfs_hld(now, e.to, u);
            }
            i++;
        }
        finish_[u] = now;
    }

    TreeType graph_;

    //  parent_[i]: 頂点iの親頂点
    std::vector<int> parent_;
    //  head_[i]: 頂点iが属している連結成分の中で最も根に近い頂点
    std::vector<int> head_;
    //  size_[i]: 頂点iを根とする部分木のサイズ
    std::vector<int> size_;
    //  depth_[i]: 頂点iの深さ
    std::vector<int> depth_;
    //  depth_[i]: 頂点iの行きがけ順
    std::vector<int> discover_;
    //  depth_[i]: 頂点iの探索が終わる時間
    std::vector<int> finish_;
};

}  //  namespace nono
