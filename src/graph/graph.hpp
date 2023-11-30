#pragma once

#include <cassert>
#include <vector>

#include "graph/edge.hpp"

namespace nono {

template <class WT = int>
class Graph {
  public:
    using EdgeType = Edge<WT>;
    using WeightType = WT;

    Graph() = default;
    explicit Graph(int vertex_size): vertex_size_(vertex_size), adj_list_(vertex_size) {}
    Graph(int vertex_size, const std::vector<EdgeType>& edges, bool directed = false)
        : vertex_size_(vertex_size),
          adj_list_(vertex_size) {
        for (const auto& e: edges) {
            adj_list_[e.from].emplace_back(e);
            if (not directed) adj_list_[e.to].emplace_back(e.inv());
        }
    }

    //  brief:
    //  - 頂点 `i` に隣接する頂点を取得する
    const std::vector<EdgeType>& operator[](int i) const {
        assert(0 <= i && i < vertex_size_);
        return adj_list_[i];
    }

    //  brief:
    //  - 頂点 `i` に隣接する頂点を取得する
    std::vector<EdgeType>& operator[](int i) {
        assert(0 <= i && i < vertex_size_);
        return adj_list_[i];
    }

    //  brief:
    //  - 頂点数を取得する
    int size() const {
        return vertex_size_;
    }

    //  brief:
    //  - グラフに無向辺を追加する
    template <class... Args>
    void add_undirected_edge(int from, int to, Args... args) {
        assert(0 <= from && from < vertex_size_);
        assert(0 <= to && to < vertex_size_);
        adj_list_[from].emplace_back(from, to, args...);
        adj_list_[to].emplace_back(to, from, args...);
    }

    //  brief:
    //  - グラフに有向辺を追加する
    template <class... Args>
    void add_directed_edge(int from, int to, Args... args) {
        assert(0 <= from && from < vertex_size_);
        assert(0 <= to && to < vertex_size_);
        adj_list_[from].emplace_back(from, to, args...);
    }

    //  brief:
    //  - 頂点 `i` の次数を取得する
    int degree(int i) const {
        assert(0 <= i && i < vertex_size_);
        return adj_list_[i].size();
    }

    //  brief:
    //  - 各頂点の入次数を得る
    std::vector<int> indegree() const {
        std::vector<int> result(vertex_size_);
        for (int i = 0; i < vertex_size_; i++) {
            for (const auto& e: adj_list_[i]) {
                result[e.to]++;
            }
        }
        return result;
    }

    //  brief:
    //  - 各頂点の出次数を得る
    std::vector<int> outdegree() const {
        std::vector<int> result(vertex_size_);
        for (int i = 0; i < vertex_size_; i++) {
            result[i] = adj_list_[i].size();
        }
        return result;
    }

  private:
    int vertex_size_;
    std::vector<std::vector<EdgeType>> adj_list_;
};

}  //  namespace nono
