#pragma once

#include <cassert>
#include <vector>

namespace nono {

///  # FenwickTree
///  一点加算区間取得. segment treeよりも定数倍が良い.
template <class T>
class FenwickTree {
  public:
    FenwickTree(int n): n_(n), data_(n_ + 1, T{0}) {}

    ///  # add(i, elem)
    ///  data[i] <= data[i] + elem
    ///  O(log n)
    void add(int i, T elem) {
        assert(0 <= i && i < n_);
        for (i++; i <= n_; i += i & -i) {
            data_[i] += elem;
        }
    }

    ///  # set(i, elem)
    ///  data[i] <= elem
    ///  O(log n)
    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        add(i, elem - get(i));
    }

    ///  # sum(i)
    ///  return sum[for j in [0, i)](data[j])
    ///  O(log n)
    T sum(int i) const {
        assert(0 <= i && i <= n_);
        T result{0};
        for (; i > 0; i -= i & -i) {
            result += data_[i];
        }
        return result;
    }

    ///  # sum(left, right)
    ///  return sum[for i in [left, right)](data[i])
    ///  O(log n)
    T sum(int left, int right) const {
        assert(left <= right);
        return sum(right) - sum(left);
    }

    ///  # get(i)
    ///  return data[i]
    ///  O(log n)
    T get(int i) const {
        assert(0 <= i && i < n_);
        return sum(i, i + 1);
    }

  private:
    int n_;
    std::vector<T> data_;
};

}  //  namespace nono
