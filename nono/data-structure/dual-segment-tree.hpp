#pragma once

#include <cassert>
#include <vector>

namespace nono {

//  brief:
//  - segment-tree. 一点更新, 範囲取得
//
//  tparam:
//  - `T`: 配列の要素の型
//  - `e`: 単位元を返す関数. 戻り値 `T`, 引数 `void` でなければならない.
//  - 'F': 写像の型
//  - 'mapping': T mapping(F, T), 写像
//  - 'composition': F mapping(F, F) 合成写像
//
//  note:
//  - 作用が可換のときしか使えない
//  - そうじゃないなら遅延セグ木で
template <class M>
class DualSegmentTree {
    using T = M::value_type;
    using F = M::func_type;

  public:
    DualSegmentTree() = default;
    DualSegmentTree(int n): n_(n), data_(n_, M::e()), lazy_(n_, M::id()) {}
    DualSegmentTree(const std::vector<T>& data): n_(data.size()), data_(data_), lazy_(n_, M::id()) {}

    void apply(int i, F value) {
        assert(0 <= i && i < n_);
        i += n_;
        eval(i, value);
    }

    void apply(int left, int right, F value) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        for (left += n_, right += n_; left < right; left >>= 1, right >>= 1) {
            if (left & 1) eval(left++, value);
            if (right & 1) eval(--right, value);
        }
    }

    T get(int i) const {
        assert(0 <= i && i < n_);
        T result = data_[i];
        for ((i += n_) >>= 1; i > 0; i >>= 1) {
            result = M::mapping(lazy_[i], result);
        }
        return result;
    }

  private:
    void eval(int i, F value) {
        if (i < n_) {
            lazy_[i] = M::composition(value, lazy_[i]);
        } else {
            data_[i - n_] = M::mapping(value, data_[i - n_]);
        }
    }

    int n_;
    std::vector<T> data_;
    std::vector<F> lazy_;
};

}  //  namespace nono
