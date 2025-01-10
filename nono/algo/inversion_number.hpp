#pragma once

#include <vector>

#include "../ds/fenwick-tree.hpp"
#include "../utility/compressor.hpp"

namespace nono {

///  # 反転数
///  i < j and A_i > A_jとなるような(i, j)の組みの数
///
///  dataがdistinctかどうか問わない(はず)
template <class T>
long long inversion_number(const std::vector<T>& data) {
    FenwickTree<long long> tree(data.size());
    long long result = 0;
    for (int i: Compressor(data).compress(data)) {
        result += tree.sum(i + 1, data.size());
        tree.add(i, 1);
    }
    return result;
}

}  //  namespace nono
