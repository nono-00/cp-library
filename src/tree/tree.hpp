#pragma once

#include <cassert>
#include <vector>

#include "graph/edge.hpp"
#include "graph/graph.hpp"
#include "tree/internal-tree-concepts.hpp"

namespace nono {

template <internal::Edge E>
class Tree: public Graph<E> {
  public:
    using typename Graph<E>::EdgeType;

    Tree() = default;
    explicit Tree(int vertex_size, int root = 0): Graph<EdgeType>(vertex_size), root_(root) {}
    explicit Tree(const std::vector<std::vector<EdgeType>>& graph, int root = 0): Graph<EdgeType>(graph), root_(root) {}
    Tree(int vertex_size, const std::vector<EdgeType>& edges, int root = 0)
        : Graph<EdgeType>(vertex_size, edges),
          root_(root) {}

    int root() const {
        return root_;
    }

    void set_root(int root) {
        assert(0 <= root && root < this->size());
        root_ = root;
    }

  private:
    int root_ = 0;
};

static_assert(internal::Tree<Tree<Edge<int>>>);

}  //  namespace nono
