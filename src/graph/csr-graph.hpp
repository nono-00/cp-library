#pragma once

#include <cassert>
#include <ranges>
#include <vector>

#include "graph/edge.hpp"
#include "graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - CSR形式のグラフ構造体
//
//  tparam:
//  - `E`: 辺の型
//
//  note:
//  - `add_edge`のようなものはない. 構築はコンストラクタでのみ可能.
template <internal::Edge E>
class CSRGraph {
    using iterator = std::vector<E>::iterator;
    using const_iterator = std::vector<E>::const_iterator;
    using subrange = std::ranges::subrange<iterator, iterator>;
    using const_subrange = std::ranges::subrange<const_iterator, const_iterator>;

  public:
    using EdgeType = E;

    CSRGraph() = default;
    CSRGraph(int vertex_size, const std::vector<EdgeType>& edges)
        : vertex_size_(vertex_size),
          csr_(edges.size()),
          start_(vertex_size_ + 1) {
        for (const auto& e: edges) {
            start_[e.from + 1]++;
        }
        for (int i = 0; i < vertex_size_; i++) {
            start_[i + 1] += start_[i];
        }
        auto index = start_;
        for (const auto& e: edges) {
            csr_[index[e.from]++] = e;
        }
    }

    subrange operator[](int pos) {
        assert(0 <= pos && pos < vertex_size_);
        return std::ranges::subrange(csr_.begin() + start_[pos], csr_.begin() + start_[pos + 1]);
    }

    const_subrange operator[](int pos) const {
        assert(0 <= pos && pos < vertex_size_);
        return std::ranges::subrange(csr_.begin() + start_[pos], csr_.begin() + start_[pos + 1]);
    }

    //  brief:
    //  - 頂点数を取得する
    int size() const {
        return vertex_size_;
    }

  private:
    int vertex_size_;
    std::vector<EdgeType> csr_;
    std::vector<int> start_;
};

static_assert(internal::Graph<CSRGraph<Edge<int>>>);

}  //  namespace nono
