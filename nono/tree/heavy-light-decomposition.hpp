#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

///  # Heavy Light Deomposition
///  maybe DEPRECATED
template <class T>
class HeavyLightDecomposition {
    static constexpr int NONE = -1;
    using Point = int;
    using Interval = std::pair<int, int>;
    using Intervals = std::vector<std::pair<int, int>>;

  public:
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

    int lowest_common_ancester(int u, int v) {
        assert(0 <= u && u < n_);
        assert(0 <= v && v < n_);
        while (head_[u] != head_[v]) {
            if (in_[u] > in_[v]) std::swap(u, v);
            v = parent_[head_[v]];
        }
        return (depth_[u] < depth_[v] ? u : v);
    }

    int distance(int u, int v) {
        return depth_[u] + depth_[v] - 2 * depth_[lowest_common_ancester(u, v)];
    }

    Point vertex(int v) {
        return in_[v];
    }

    Point edge(int u, int v) {
        if (depth_[u] > depth_[v]) std::swap(u, v);
        assert(parent_[v] == u);
        return in_[v];
    }

    Interval vertices_for_subtree(int u) {
        return subtree(u, true);
    }

    Interval edges_for_subtree(int u) {
        return subtree(u, false);
    }

    Intervals vertices_for_path(int u, int v) {
        return path(u, v, true);
    }

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
