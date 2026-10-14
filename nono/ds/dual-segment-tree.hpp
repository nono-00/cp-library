#pragma once

#include <cassert>
#include <vector>

namespace nono {

//  # DualSegmentTree
//  双対セグ木。区間作用, 一点取得のsegment tree.
//  Mはact-monoid。作用が可換でないと壊れる
template <class M>
class DualSegmentTree {
    using T = M::Value;
    using F = M::Act;

  public:
    DualSegmentTree() = default;
    //  # DualSegmentTree(n)
    //  長さnで、全要素を M::e() にする
    DualSegmentTree(int n): n_(n), data_(n_, M::e()), lazy_(n_, M::id()) {}
    //  # DualSegmentTree(data)
    //  dataの内容で初期化する
    DualSegmentTree(const std::vector<T>& data): n_(data.size()), data_(data), lazy_(n_, M::id()) {}

    //  # apply(i, value)
    //  data[i] <= mapping(value, data[i])
    //  O(log n)
    void apply(int i, F value) {
        assert(0 <= i && i < n_);
        i += n_;
        eval(i, value);
    }

    //  # apply(left, right, value)
    //  [for i in [left, right)](data[i] <= mapping(value, data[i]))
    //  0 <= left <= right <= n でないと壊れる
    //  O(log n)
    void apply(int left, int right, F value) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        for (left += n_, right += n_; left < right; left >>= 1, right >>= 1) {
            if (left & 1) eval(left++, value);
            if (right & 1) eval(--right, value);
        }
    }

    //  # get(i)
    //  return data[i]
    //  O(log n)
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
