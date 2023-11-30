#pragma once

#include "data-structure/fenwick-tree.hpp"
#include "utility/compressor.hpp"

namespace nono {

//  brief:
//  - 反転数
//
//  complexity:
//  - O(NlogN)
//
//  note:
//  - i < jかつA_i > A_jとなるような(i, j)の組みの数
template <class T>
long long inversion_number(const std::vector<T>& data) {
    FenwickTree<int> fenwicktree(data.size());
    long long result = 0;
    for (int i: Compressor(data).compress(data)) {
        result += fenwicktree.sum(i, data.size());
        fenwicktree.add(i, 1);
    }
    return result;
}

}  //  namespace nono
