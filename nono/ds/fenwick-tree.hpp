#pragma once

#include <cassert>
#include <vector>

namespace nono {

/// brief : 一点加算区間取得. segment treeよりも定数倍が良い.
/// TODO : 二分探索
template <class T>
class FenwickTree {
  public:
    FenwickTree(int n): n_(n), data_(n_ + 1, T{0}) {}

    void add(int i, T elem) {
        assert(0 <= i && i < n_);
        for (i++; i <= n_; i += i & -i) {
            data_[i] = data_[i] + elem;
        }
    }

    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        add(i, elem - get(i));
    }

    // [0, i)の総和
    T sum(int i) const {
        assert(0 <= i && i <= n_);
        T result{0};
        for (; i > 0; i -= i & -i) {
            result = result + data_[i];
        }
        return result;
    }

    // [left, right)
    T sum(int left, int right) const {
        assert(left <= right);
        return sum(right) - sum(left);
    }

    T get(int i) const {
        assert(0 <= i && i < n_);
        return sum(i, i + 1);
    }

  private:
    int n_;
    std::vector<T> data_;
};

}  //  namespace nono
