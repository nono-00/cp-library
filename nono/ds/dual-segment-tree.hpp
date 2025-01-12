#pragma once

#include <cassert>
#include <vector>

namespace nono {

///  # 双対セグ木
///  区間作用, 一点取得のsegment tree.
///  作用が可換のもののみに対応.
template <class M>
class DualSegmentTree {
    using T = M::Value;
    using F = M::Act;

  public:
    DualSegmentTree() = default;
    DualSegmentTree(int n): n_(n), data_(n_, M::e()), lazy_(n_, M::id()) {}
    DualSegmentTree(const std::vector<T>& data): n_(data.size()), data_(data_), lazy_(n_, M::id()) {}

    ///  # apply(i, value)
    ///  data[i] <= mapping(value, data[i])
    ///  O(logn)
    void apply(int i, F value) {
        assert(0 <= i && i < n_);
        i += n_;
        eval(i, value);
    }

    ///  # apply(l, r, value)
    ///  forall i in [l, r) data[i] <= mapping(value, data[i])
    ///  O(logn)
    void apply(int left, int right, F value) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        for (left += n_, right += n_; left < right; left >>= 1, right >>= 1) {
            if (left & 1) eval(left++, value);
            if (right & 1) eval(--right, value);
        }
    }

    ///  # get(i)
    ///  return data[i]
    ///  O(logn)
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
