#pragma once

#include "nono/graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - Edge構造体
//
//  tparam:
//  - `T`: 重みの型
template <class T = void>
struct Edge {
    using WeightType = T;

    int from;
    int to;
    T weight;
    int id;

    Edge() {}
    Edge(int from, int to, T weight = T(), int id = -1): from(from), to(to), weight(weight), id(id) {}
};

static_assert(internal::WeightedIndexedEdge<Edge<int>>);

}  //  namespace nono
