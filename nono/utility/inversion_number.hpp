#pragma once

#include "nono/data-structure/fenwick-tree.hpp"
#include "nono/utility/compressor.hpp"

namespace nono {

template <class T>
struct Add {
    using value_type = T;
    static value_type op(value_type lhs, value_type rhs) {
        return lhs + rhs;
    }
    static value_type e() {
        return 0;
    }
    static value_type inv(value_type value) {
        return -value;
    }
};

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
    FenwickTree<Add<T>> fenwicktree(data.size());
    long long result = 0;
    for (int i: Compressor(data).compress(data)) {
        result += fenwicktree.prod(i, data.size());
        fenwicktree.apply(i, 1);
    }
    return result;
}

}  //  namespace nono
