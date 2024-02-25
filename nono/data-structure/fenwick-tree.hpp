#pragma once

#include <cassert>
#include <vector>

namespace nono {

//  brief:
//  - FenwickTree. 区間和の取得が可能.
//
//  note:
//  - `T` が数値型ならOK.
//  - 数値型でなかったとしても
//  - 1. ゼロ初期化によって単位元を得られる
//  - 2. `+`, `-` が定義されている
//  - ならOK.
//  
//  todo:
//  - FenwickTree上の二分探索
template <class T>
class FenwickTree {
  public:
    FenwickTree(int size = 0): size_(size), data_(size_ + 1) {}

    //  brief:
    //  - `i` 番目の要素に `v` を加算する
    void add(int i, const T& v) {
        assert(0 <= i && i < size_);
        for (++i; i <= size_; i += i & -i) {
            data_[i] += v;
        }
    }

    //  brief:
    //  - `[0, i)` の和を取得する
    T sum(int i) const {
        assert(0 <= i && i <= size_);
        T result{};
        for (; i > 0; i -= i & -i) {
            result += data_[i];
        }
        return result;
    }

    //  brief:
    //  - `[l, r)` の和を取得する
    T sum(int l, int r) const {
        assert(0 <= l && l <= size_);
        assert(l <= r && r <= size_);
        return sum(r) - sum(l);
    }

  private:
    int size_;
    std::vector<T> data_;
};

}  //  namespace nono
