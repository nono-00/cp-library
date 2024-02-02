#pragma once

#include <vector>
#include <iterator>

#include "math/zeta-mobius-transform.hpp"

namespace nono {

template <class T>
std::vector<T> bitwise_or_convolution(std::vector<T> lhs, std::vector<T> rhs) {
    assert(std::size(lhs) == std::size(rhs));
    int size = std::size(lhs);
    subset_zeta_transform(lhs);
    subset_zeta_transform(rhs);
    for (int i = 0; i < size; i++) lhs[i] *= rhs[i];
    subset_mobius_transform(lhs);
    return lhs;
}

}  //  namespace nono
