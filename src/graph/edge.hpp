#pragma once

namespace nono {

//  brief:
//  - Edge構造体
//
//  tparam:
//  - `T`: 重みの型
template <class T = int>
struct Edge {
    using WeightType = T;

    int from;
    int to;
    T weight;
    int id;

    Edge() = default;

    Edge(int from, int to, const T weight = 1, int id = -1): from(from), to(to), weight(weight), id(id) {}
    Edge inv() const {
        return {to, from, weight, id};
    }
};

}  //  namespace nono
