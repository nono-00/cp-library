#pragma once

#include <cassert>
#include <vector>

#include "nono/graph/edge.hpp"
#include "nono/graph/internal-graph-concepts.hpp"

namespace nono {

template <internal::Edge E>
class Graph {
    using iterator = std::vector<E>::iterator;
    using const_iterator = std::vector<E>::const_iterator;

  public:
    using EdgeType = E;

    Graph() = default;
    explicit Graph(int vertex_size): vertex_size_(vertex_size), adj_(vertex_size) {}
    explicit Graph(const std::vector<std::vector<EdgeType>>& graph): adj_(graph) {}
    Graph(int vertex_size, const std::vector<EdgeType>& edges): vertex_size_(vertex_size), adj_(vertex_size) {
        for (const auto& e: edges) {
            adj_[e.from].emplace_back(e);
        }
    }

    //  brief:
    //  - 頂点 `i` に隣接する頂点を取得する
    const std::vector<EdgeType>& operator[](int pos) const {
        assert(0 <= pos && pos < vertex_size_);
        return adj_[pos];
    }

    //  brief:
    //  - 頂点 `i` に隣接する頂点を取得する
    std::vector<EdgeType>& operator[](int pos) {
        assert(0 <= pos && pos < vertex_size_);
        return adj_[pos];
    }

    //  brief:
    //  - 頂点数を取得する
    int size() const {
        return vertex_size_;
    }

    iterator begin() {
        return adj_.begin();
    }

    const_iterator begin() const {
        return adj_.begin();
    }

    iterator end() {
        return adj_.end();
    }

    const_iterator end() const {
        return adj_.end();
    }

    //  brief:
    //  - グラフに辺を追加する
    void add_edge(const EdgeType& edge) {
        assert(0 <= edge.from && edge.from < vertex_size_);
        assert(0 <= edge.to && edge.to < vertex_size_);
        adj_[edge.from].emplace_back(edge);
    }

  private:
    int vertex_size_;
    std::vector<std::vector<EdgeType>> adj_;
};

static_assert(internal::Graph<Graph<Edge<int>>>);

}  //  namespace nono
