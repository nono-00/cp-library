#pragma once

#include <cassert>

#include "nono/ds/fenwick-tree.hpp"

namespace nono {

///  brief : 区間加算区間取得 Fenwick Tree. lazy segment tree よりも定数倍が良い(はず).
template <class T>
class RangeAddFenwickTree {
  public:
    RangeAddFenwickTree(int n = 0): n_(n), data1_(n), data2_(n) {}

    //  brief:
    //  - `i` 番目の要素に `v` を加算する
    void add(int i, T elem) {
        assert(0 <= i && i < n_);
        data1_.add(i, elem);
    }

    //  brief:
    //  - 区間 `[l, r)` の各要素に `v` を加算する
    void add(int left, int right, T elem) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        data1_.add(left, -left * elem);
        data2_.add(left, elem);
        if (right < n_) {
            data1_.add(right, right * elem);
            data2_.add(right, -elem);
        }
    }

    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        add(i, elem - get(i));
    }

    //  brief:
    //  - 区間 `[0, i)` の要素の総和を取得する
    T sum(int i) const {
        assert(0 <= i && i <= n_);
        return data1_.sum(i) + i * data2_.sum(i);
    }

    //  brief:
    //  - 区間 `[l, r)` の要素の総和を取得する
    T sum(int left, int right) const {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        return sum(right) - sum(left);
    }

    T get(int i) const {
        assert(0 <= i && i < n_);
        return sum(i, i + 1);
    }

  private:
    int n_;
    FenwickTree<T> data1_;
    FenwickTree<T> data2_;
};

}  //  namespace nono
