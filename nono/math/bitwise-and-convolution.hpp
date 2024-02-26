#pragma once

#include <iterator>
#include <vector>

#include "nono/math/zeta-mobius-transform.hpp"

namespace nono {

template <class T>
std::vector<T> bitwise_and_convolution(std::vector<T> lhs, std::vector<T> rhs) {
    assert(std::size(lhs) == std::size(rhs));
    int size = std::size(lhs);
    superset_zeta_transform(lhs);
    superset_zeta_transform(rhs);
    for (int i = 0; i < size; i++) lhs[i] *= rhs[i];
    superset_mobius_transform(lhs);
    return lhs;
}

}  //  namespace nono
