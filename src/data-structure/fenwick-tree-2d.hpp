#pragma once

#include <cassert>
#include <vector>

namespace nono {

//  brief:
//  - 二次元FenwickTree. 一点更新、総和取得が O((logN) ^ 2) で可能
template <class T>
class FenwickTree2D {
  public:
    FenwickTree2D() = default;
    FenwickTree2D(int height, int width)
        : height_(height),
          width_(width),
          data_(height_ + 1, std::vector<T>(width_ + 1)) {}

    //  brief:
    //  - `(h, w)` に `v` を加算する
    //
    //  complexity:
    //  - O(logH logW)
    void add(int h, int w, const T& v) {
        assert(0 <= h && h <= height_);
        assert(0 <= w && w <= width_);
        for (int i = h + 1; i <= height_; i += i & -i) {
            for (int j = w + 1; j <= width_; j += j & -j) {
                data_[i][j] += v;
            }
        }
    }

    //  brief:
    //  - `[0, h) \times [0, w)` の総和を取得する
    T sum(int h, int w) {
        assert(0 <= h && h <= height_);
        assert(0 <= w && w <= width_);
        T result = {};
        for (int i = h; i > 0; i -= i & -i) {
            for (int j = w; j > 0; j -= j & -j) {
                result += data_[i][j];
            }
        }
        return result;
    }

    //  brief:
    //  - `[lh, rh) \times [lw, rw)` の総和を取得する
    T sum(int lh, int lw, int rh, int rw) {
        assert(0 <= lh && lh <= height_);
        assert(0 <= lw && lw <= width_);
        assert(lh <= rh && rh <= height_);
        assert(lh <= rw && rw <= width_);
        return sum(rh, rw) - sum(rh, lw) - sum(lh, rw) + sum(lh, lw);
    }

  private:
    int height_;
    int width_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
