#pragma once

#include <cassert>
#include <vector>

namespace nono {

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
