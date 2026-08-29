#pragma once

#include <cassert>
#include <vector>

namespace nono {

///  # 二次元FenwickTree
///  二次元領域和を扱う
template <class T>
class FenwickTree2D {
  public:
    FenwickTree2D() = default;
    ///  # FenwickTree2D(h, w)
    ///  data[h][w]
    ///  O(h * w)
    FenwickTree2D(int h, int w): h_(h), w_(w), data_(h_ + 1, std::vector<T>(w_ + 1, T{0})) {}

    ///  # add(h, w, elem)
    ///  data[h][w] <= data[h][w] + elem
    ///  O(logh logw)
    void add(int h, int w, T elem) {
        assert(0 <= h && h < h_);
        assert(0 <= w && w < w_);
        for (int i = h + 1; i <= h_; i += i & -i) {
            for (int j = w + 1; j <= w_; j += j & -j) {
                data_[i][j] += elem;
            }
        }
    }

    ///  # set(h, w, elem)
    ///  data[h][w] <= elem
    ///  O(logh logw)
    void set(int h, int w, T elem) {
        assert(0 <= h && h < h_);
        assert(0 <= w && w < w_);
        add(h, w, elem - get(h, w));
    }

    ///  # sum(h, w)
    ///  return sum[for i in [0, h), for j in [0, w)](data[i][j])
    ///  O(logh logw)
    T sum(int h, int w) const {
        assert(0 <= h && h <= h_);
        assert(0 <= w && w <= w_);
        T result{0};
        for (int i = h; i > 0; i -= i & -i) {
            for (int j = w; j > 0; j -= j & -j) {
                result += data_[i][j];
            }
        }
        return result;
    }

    ///  # sum(lh, lw, rh, rw)
    ///  return sum[for i in [lh, rh), for j in [lw, rw)](data[i][j])
    ///  O(logh logw)
    T sum(int lh, int lw, int rh, int rw) const {
        assert(0 <= lh && lh <= h_);
        assert(0 <= lw && lw <= w_);
        assert(lh <= rh && rh <= h_);
        assert(lw <= rw && rw <= w_);
        return sum(rh, rw) + sum(lh, lw) - sum(rh, lw) - sum(lh, rw);
    }

    ///  # get(h, w)
    ///  return data[h][w]
    ///  O(logh logw)
    T get(int h, int w) const {
        assert(0 <= h && h < h_);
        assert(0 <= w && w < w_);
        return sum(h, w, h + 1, w + 1);
    }

  private:
    int h_;
    int w_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
