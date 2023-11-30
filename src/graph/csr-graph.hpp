#pragma once

#include <cassert>
#include <ranges>
#include <vector>

#include "graph/edge.hpp"

namespace nono {

//  brief:
//  - CSR形式のグラフ構造体
//
//  tparam:
//  - `Edge<WT>`: 辺の型
//
//  note:
//  - `add_edge`のようなものはない. 構築はコンストラクタでのみ可能.
template <class WT = int>
class CSRGraph {
    using iterator = std::vector<Edge<WT>>::iterator;
    using const_iterator = std::vector<Edge<WT>>::const_iterator;
    using subrange = std::ranges::subrange<iterator, iterator>;
    using const_subrange = std::ranges::subrange<const_iterator, const_iterator>;

  public:
    using EdgeType = Edge<WT>;
    using WeightType = WT;

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

    subrange operator[](int i) {
        assert(0 <= i && i < vertex_size_);
        return std::ranges::subrange(csr_.begin() + start_[i], csr_.begin() + start_[i + 1]);
    }

    const_subrange operator[](int i) const {
        assert(0 <= i && i < vertex_size_);
        return std::ranges::subrange(csr_.begin() + start_[i], csr_.begin() + start_[i + 1]);
    }

    //  brief:
    //  - 頂点数を取得する
    int size() const {
        return vertex_size_;
    }

    //  brief:
    //  - 頂点 `i` の次数を取得する
    int degree(int i) const {
        assert(0 <= i && i < vertex_size_);
        return start_[i + 1] - start_[i];
    }

    //  brief:
    //  - 各頂点の入次数を得る
    std::vector<int> indegree() const {
        std::vector<int> result(vertex_size_);
        for (const auto& e: csr_) {
            result[e.to]++;
        }
        return result;
    }

    //  brief:
    //  - 各頂点の出次数を得る
    std::vector<int> outdegree() const {
        std::vector<int> result(vertex_size_);
        for (const auto& e: csr_) {
            result[e.from]++;
        }
        return result;
    }

  private:
    int vertex_size_;
    std::vector<EdgeType> csr_;
    std::vector<int> start_;
};

}  //  namespace nono
