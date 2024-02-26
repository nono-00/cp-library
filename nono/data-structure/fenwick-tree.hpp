#pragma once

#include <cassert>
#include <vector>

namespace nono {

template <class G>
class FenwickTree {
    using T = G::value_type;

  public:
    FenwickTree(int n): n_(n), data_(n_ + 1, G::e()) {}

    void apply(int i, T elem) {
        assert(0 <= i && i < n_);
        for (i++; i <= n_; i += i & -i) {
            data_[i] = G::op(data_[i], elem);
        }
    }

    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        apply(i, G::op(elem, G::inv(get(i))));
    }

    T prod(int i) const {
        assert(0 <= i && i <= n_);
        T result = G::e();
        for (; i > 0; i -= i & -i) {
            result = G::op(result, data_[i]);
        }
        return result;
    }

    T prod(int left, int right) const {
        assert(left <= right);
        return G::op(prod(right), G::inv(prod(left)));
    }

    T get(int i) const {
        assert(0 <= i && i < n_);
        return prod(i, i + 1);
    }

  private:
    int n_;
    std::vector<T> data_;
};

}  //  namespace nono
