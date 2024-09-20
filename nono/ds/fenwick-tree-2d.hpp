#pragma once

#include <cassert>
#include <vector>

namespace nono {

///  brief : 一点加算、総和取得が \\( O(\log ^ 2 N) \\) で可能
template <class T>
class FenwickTree2D {
  public:
    FenwickTree2D() = default;
    FenwickTree2D(int h, int w): h_(h), w_(w), data_(h_ + 1, std::vector<T>(w_ + 1, T{0})) {}

    //  brief:
    //  - `(h, w)` に `v` を加算する
    //
    //  complexity:
    //  - O(logH logW)
    void add(int h, int w, T elem) {
        assert(0 <= h && h < h_);
        assert(0 <= w && w < w_);
        for (int i = h + 1; i <= h_; i += i & -i) {
            for (int j = w + 1; j <= w_; j += j & -j) {
                data_[i][j] += elem;
            }
        }
    }

    void set(int h, int w, T elem) {
        assert(0 <= h && h < h_);
        assert(0 <= w && w < w_);
        add(h, w, elem - get(h, w));
    }

    //  brief:
    //  - `[0, h) \times [0, w)` の総和を取得する
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

    //  brief:
    //  - `[lh, rh) \times [lw, rw)` の総和を取得する
    T sum(int lh, int lw, int rh, int rw) const {
        assert(0 <= lh && lh <= h_);
        assert(0 <= lw && lw <= w_);
        assert(lh <= rh && rh <= h_);
        assert(lw <= rw && rw <= w_);
        return sum(rh, rw) + sum(lh, lw) - sum(rh, lw) - sum(lh, rw);
    }

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
