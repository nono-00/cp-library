#pragma once

#include <vector>
#include <cassert>

#include "data-structure/fenwick-tree.hpp"

namespace nono {

//  brief:
//  - 区間加算可能な `FenwickTree` 
//
//  depend:
//  - `FenwickTree`
template <class T>
class RangeAddFenwickTree {
public:
    RangeAddFenwickTree(int size = 0): size_(size), data1_(size), data2_(size) {}

    //  brief:
    //  - `i` 番目の要素に `v` を加算する
    void add(int i, const T& v) {
        assert(0 <= i && i < size_);
        data1_.add(i, v);
    }

    //  brief:
    //  - 区間 `[l, r)` の各要素に `v` を加算する
    void add(int l, int r, const T& v) {
        assert(0 <= l && l <= size_);
        assert(l <= r && r <= size_);
        data1_.add(l, -l * v);
        data2_.add(l, v);
        if (r < size_) {
            data1_.add(r, r * v);
            data2_.add(r, -v);
        }
    }

    //  brief:
    //  - 区間 `[0, i)` の要素の総和を取得する
    T sum(int i) const {
        assert(0 <= i && i <= size_);
        return data1_.sum(i) + i * data2_.sum(i);
    }

    //  brief:
    //  - 区間 `[l, r)` の要素の総和を取得する 
    T sum(int l, int r) const {
        assert(0 <= l && l <= size_);
        assert(l <= r && r <= size_);
        return sum(r) - sum(l);
    }

private:
    int size_;
    FenwickTree<T> data1_;
    FenwickTree<T> data2_;
};

}  //  namespace nono
