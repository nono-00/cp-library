#pragma once

#include <iterator>
#include <vector>

#include "./zeta-mobius-transform.hpp"

namespace nono {

//  # bitwise_or_convolution(lhs, rhs)
//  result[k] = sum[for (i, j) with (i | j) = k](lhs[i] * rhs[j]) となる列を返す
//  lhsとrhsの長さが同じ2冪でないと壊れる
//  O(n log n)
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
