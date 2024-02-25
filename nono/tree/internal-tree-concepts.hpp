#pragma once

#include "graph/internal-graph-concepts.hpp"

namespace nono {

namespace internal {

template <class T>
concept Tree = internal::Graph<T> && requires(T t) {
    t.root();
    t.set_root(std::declval<int>());
};

template <class T>
concept WeightedTree = internal::Tree<T> && internal::WeightedEdge<typename T::EdgeType>;

template <class T>
concept IndexedTree = internal::Tree<T> && internal::IndexedEdge<typename T::EdgeType>;

template <class T>
concept WeightedIndexedTree = internal::Tree<T> && internal::WeightedIndexedEdge<typename T::EdgeType>;

}  //  namespace internal

}  //  namespace nono
