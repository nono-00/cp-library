#pragma once

#include <ranges>
#include <type_traits>

namespace nono {

namespace internal {

template <class E>
concept Edge = requires(E e) {
    e.from;
    e.to;
};

template <class E>
concept WeightedEdge = internal::Edge<E> && requires(E e) {
    typename E::WeightType;
    e.weight;
};

template <class E>
concept IndexedEdge = internal::Edge<E> && requires(E e) { e.id; };

template <class E>
concept WeightedIndexedEdge = internal::WeightedEdge<E> && internal::IndexedEdge<E>;

template <class G>
concept Graph = internal::Edge<typename G::EdgeType> && requires(G g) { 
    requires std::ranges::random_access_range<decltype(g[std::declval<int>()])>;
    g.size(); 
};

template <class G>
concept WeightedGraph = internal::Graph<G> && internal::WeightedEdge<typename G::EdgeType>;

template <class G>
concept IndexedGraph = internal::Graph<G> && internal::IndexedEdge<typename G::EdgeType>;

template <class G>
concept WeightedIndexedGraph = internal::WeightedGraph<G> && internal::IndexedGraph<G>;

}  //  namespace internal

}  //  namespace nono
