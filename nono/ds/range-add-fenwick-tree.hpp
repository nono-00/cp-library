#pragma once

#include <cassert>

#include "nono/data-structure/fenwick-tree.hpp"

namespace nono {

//  brief:
//  - 区間加算可能な `FenwickTree`
//
//  すまん、壊れている
//  困った
template <class G>
class RangeAddFenwickTree {
    using T = G::value_type;

  public:
    RangeAddFenwickTree(int n = 0): n_(n), data1_(n), data2_(n) {}

    //  brief:
    //  - `i` 番目の要素に `v` を加算する
    void apply(int i, T elem) {
        assert(0 <= i && i < n_);
        data1_.apply(i, elem);
    }

    //  brief:
    //  - 区間 `[l, r)` の各要素に `v` を加算する
    void apply(int left, int right, T elem) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        data1_.apply(left, G::inv(left) * elem);
        data2_.apply(left, elem);
        if (right < n_) {
            data1_.apply(right, right * elem);
            data2_.apply(right, G::inv(elem));
        }
    }

    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        apply(i, G::op(elem, G::inv(get(i))));
    }

    //  brief:
    //  - 区間 `[0, i)` の要素の総和を取得する
    T prod(int i) const {
        assert(0 <= i && i <= n_);
        return G::op(data1_.prod(i), i * data2_.prod(i));
    }

    //  brief:
    //  - 区間 `[l, r)` の要素の総和を取得する
    T prod(int left, int right) const {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        return G::op(prod(right), G::inv(prod(left)));
    }

    T get(int i) const {
        assert(0 <= i && i < n_);
        return prod(i, i + 1);
    }

  private:
    int n_;
    FenwickTree<G> data1_;
    FenwickTree<G> data2_;
};

}  //  namespace nono
