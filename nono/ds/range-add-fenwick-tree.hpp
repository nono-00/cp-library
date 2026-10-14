#pragma once

#include <cassert>

#include "./fenwick-tree.hpp"

namespace nono {

//  # RangeAddFenwickTree
//  区間加算区間取得 Fenwick Tree.
template <class T>
class RangeAddFenwickTree {
  public:
    //  # RangeAddFenwickTree(n)
    //  長さnで、全要素を0にする
    RangeAddFenwickTree(int n = 0): n_(n), data1_(n), data2_(n) {}

    //  # add(i, elem)
    //  data[i] <= data[i] + elem
    //  O(log n)
    void add(int i, T elem) {
        assert(0 <= i && i < n_);
        data1_.add(i, elem);
    }

    //  # add(left, right, elem)
    //  [for i in [left, right)](data[i] <= data[i] + elem)
    //  0 <= left <= right <= n でないと壊れる
    //  O(log n)
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

    //  # set(i, elem)
    //  data[i] <= elem
    //  O(log n)
    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        add(i, elem - get(i));
    }

    //  # sum(i)
    //  return sum[for j in [0, i)](data[j])
    //  O(log n)
    T sum(int i) const {
        assert(0 <= i && i <= n_);
        return data1_.sum(i) + i * data2_.sum(i);
    }

    //  # sum(left, right)
    //  return sum[for i in [left, right)](data[i])
    //  0 <= left <= right <= n でないと壊れる
    //  O(log n)
    T sum(int left, int right) const {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        return sum(right) - sum(left);
    }

    //  # get(i)
    //  return data[i]
    //  O(log n)
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
