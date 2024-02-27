#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <vector>

namespace nono {

template <class M>
class LazySegmentTree {
    using T = M::value_type;
    using F = M::func_type;

  public:
    LazySegmentTree(int n): n_(n), log_(std::bit_width((unsigned)n_)), data_(2 * n_, M::e()), lazy_(n_, M::id()) {}
    LazySegmentTree(const std::vector<T>& data)
        : n_(data.size()),
          log_(std::bit_width((unsigned)n_)),
          data_(2 * n_, M::e()),
          lazy_(n_, M::id()) {
        std::copy(data.begin(), data.end(), data_.begin() + n_);
        for (int i = n_ - 1; i > 0; i--) {
            update(i);
        }
    }

    void set(int i, T value) {
        assert(0 <= i && i < n_);
        i += n_;
        for (int j = log_; j > 0; j--) {
            if ((i >> j) << j != i) push(i >> j);
        }
        data_[i] = value;
        for (int j = 1; j <= log_; j++) update(i >> j);
    }

    void apply(int i, F value) {
        assert(0 <= i && i < n_);
        i += n_;
        for (int j = log_; j > 0; j--) {
            if ((i >> j) << j != i) push(i >> j);
        }
        data_[i] = M::mapping(value, data_[i]);
        for (int j = 1; j <= log_; j++) update(i >> j);
    }

    void apply(int left, int right, F value) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        left += n_;
        right += n_;
        for (int i = log_; i > 0; i--) {
            if ((left >> i) << i != left) push(left >> i);
            if ((right >> i) << i != right) push((right - 1) >> i);
        }
        int left_ = left;
        int right_ = right;
        for (; left < right; left >>= 1, right >>= 1) {
            if (left & 1) eval(left++, value);
            if (right & 1) eval(--right, value);
        }
        left = left_;
        right = right_;
        for (int i = 1; i <= log_; i++) {
            if ((left >> i) << i != left) update(left >> i);
            if ((right >> i) << i != right) update((right - 1) >> i);
        }
    }

    T prod(int left, int right) {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        left += n_;
        right += n_;
        for (int i = log_; i > 0; i--) {
            if ((left >> i) << i != left) push(left >> i);
            if ((right >> i) << i != right) push((right - 1) >> i);
        }
        T l_result = M::e();
        T r_result = M::e();
        for (; left < right; left >>= 1, right >>= 1) {
            if (left & 1) l_result = M::op(l_result, data_[left++]);
            if (right & 1) r_result = M::op(data_[--right], r_result);
        }
        return M::op(l_result, r_result);
    }

    T get(int i) {
        assert(0 <= i && i < n_);
        i += n_;
        for (int j = log_; j > 0; j--) {
            if ((i >> j) << j != i) push(i >> j);
        }
        return data_[i];
    }

  private:
    void update(int i) {
        assert(0 <= i && i < n_);
        data_[i] = M::op(data_[2 * i], data_[2 * i + 1]);
    }

    void eval(int i, F value) {
        data_[i] = M::mapping(value, data_[i]);
        if (i < n_) lazy_[i] = M::composition(value, lazy_[i]);
    }

    void push(int i) {
        assert(0 <= i && i < n_);
        if (lazy_[i] != M::id()) {
            data_[2 * i] = M::mapping(lazy_[i], data_[2 * i]);
            if (2 * i < n_) lazy_[2 * i] = M::composition(lazy_[i], lazy_[2 * i]);
            data_[2 * i + 1] = M::mapping(lazy_[i], data_[2 * i + 1]);
            if (2 * i + 1 < n_) lazy_[2 * i + 1] = M::composition(lazy_[i], lazy_[2 * i + 1]);
            lazy_[i] = M::id();
        }
    }

    int n_;
    int log_;
    std::vector<T> data_;
    std::vector<F> lazy_;
};

}  //  namespace nono
