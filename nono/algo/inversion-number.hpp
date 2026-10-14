#pragma once

#include <vector>

#include "../ds/fenwick-tree.hpp"
#include "../utility/compressor.hpp"

namespace nono {

//  # inversion_number(data)
//  転倒数。i < j かつ data[i] > data[j] となる組 (i, j) の個数を返す
//  値が等しい組は数えない
//  O(n log n)
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
