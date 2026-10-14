#pragma once

#include <cassert>
#include <vector>

namespace nono {

//  # superset_zeta_transform(seq)
//  添字を集合とみなした高速ゼータ変換(上位集合)。seqを書き換える
//  seq[s] <- sum[for t ⊇ s](seq[t])
//  seqの長さが2冪でないと壊れる
//  O(n log n)
template <class T>
void superset_zeta_transform(std::vector<T>& seq) {
    int size = std::size(seq);
    assert((size & (size - 1)) == 0);
    for (int i = 1; i < size; i <<= 1) {
        for (int state = 0; state < size; state++) {
            if (state & i) seq[state ^ i] += seq[state];
        }
    }
}

//  # superset_mobius_transform(seq)
//  superset_zeta_transformの逆変換。seqを書き換える
//  seqの長さが2冪でないと壊れる
//  O(n log n)
template <class T>
void superset_mobius_transform(std::vector<T>& seq) {
    int size = std::size(seq);
    assert((size & (size - 1)) == 0);
    for (int i = 1; i < size; i <<= 1) {
        for (int state = 0; state < size; state++) {
            if (state & i) seq[state ^ i] -= seq[state];
        }
    }
}

//  # subset_zeta_transform(seq)
//  添字を集合とみなした高速ゼータ変換(部分集合)。seqを書き換える
//  seq[s] <- sum[for t ⊆ s](seq[t])
//  seqの長さが2冪でないと壊れる
//  O(n log n)
template <class T>
void subset_zeta_transform(std::vector<T>& seq) {
    int size = std::size(seq);
    assert((size & (size - 1)) == 0);
    for (int i = 1; i < size; i <<= 1) {
        for (int state = 0; state < size; state++) {
            if (state & i) seq[state] += seq[state ^ i];
        }
    }
}

//  # subset_mobius_transform(seq)
//  subset_zeta_transformの逆変換。seqを書き換える
//  seqの長さが2冪でないと壊れる
//  O(n log n)
template <class T>
void subset_mobius_transform(std::vector<T>& seq) {
    int size = std::size(seq);
    assert((size & (size - 1)) == 0);
    for (int i = 1; i < size; i <<= 1) {
        for (int state = 0; state < size; state++) {
            if (state & i) seq[state] -= seq[state ^ i];
        }
    }
}

}  //  namespace nono
