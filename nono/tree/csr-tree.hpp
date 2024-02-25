#pragma once

#include <cassert>
#include <vector>

#include "graph/csr-graph.hpp"
#include "graph/edge.hpp"
#include "tree/internal-tree-concepts.hpp"

namespace nono {

//  brief:
//  - CSR形式のグラフ構造体
//
//  tparam:
//  - `E`: 辺の型
template <internal::Edge E>
class CSRTree: public CSRGraph<E> {
  public:
    CSRTree() = default;
    CSRTree(int vertex_size, const std::vector<E>& edges, int root = 0)
        : CSRGraph<E>(vertex_size, edges),
          root_(root) {}

    int root() const {
        return root_;
    }

    void set_root(int root) {
        assert(0 <= root && root < this->size());
        root_ = root;
    }

  private:
    int root_;
};

static_assert(internal::Tree<CSRTree<Edge<int>>>);

}  //  namespace nono
