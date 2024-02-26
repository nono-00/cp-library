#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <vector>

namespace nono {

template <class M>
class SegmentTree2D {
    using T = M::value_type;

  public:
    SegmentTree2D(int h, int w)
        : input_h_(h),
          input_w_(w),
          size_h_(std::bit_ceil((unsigned)h)),
          size_w_(std::bit_ceil((unsigned)w)),
          data_(2 * size_h_, std::vector<T>(2 * size_w_, M::e())),
          log_table(std::max(2 * size_h_, 2 * size_w_)) {
        for (int log = 0, n = log_table.size(); (1 << log) < n; log++) {
            for (int i = 1 << log; i < (2 << log); i++) {
                log_table[i] = log;
            }
        }
    }

    SegmentTree2D(const std::vector<std::vector<T>>& data)
        : input_h_(data.size()),
          input_w_(data.front().size()),
          size_h_(std::bit_ceil(data.size())),
          size_w_(std::bit_ceil(data.front().size())),
          data_(2 * size_h_, std::vector<T>(2 * size_w_, M::e())),
          log_table(std::max(2 * size_h_, 2 * size_w_)) {
        for (int log = 0, n = log_table.size(); (1 << log) < n; log++) {
            for (int i = 1 << log; i < (2 << log); i++) {
                log_table[i] = log;
            }
        }
        for (int i = 0; i < input_h_; i++) {
            std::copy(data[i].begin(), data[i].end(), data_[size_h_ + i].begin() + size_w_);
        }
        for (int i = 2 * size_h_ - 1; i > 0; i--) {
            for (int j = 2 * size_w_ - 1; j > 0; j--) {
                if (i >= size_h_ && j >= size_w_) continue;
                if (log_table[size_h_] - log_table[i] == log_table[size_w_] - log_table[j]) {
                    data_[i][j] = M::op(M::op(data_[2 * i][2 * j], data_[2 * i + 1][2 * j + 1]),
                                        M::op(data_[2 * i + 1][2 * j], data_[2 * i][2 * j + 1]));
                } else if (log_table[size_h_] - log_table[i] > log_table[size_w_] - log_table[j]) {
                    data_[i][j] = M::op(data_[2 * i][j], data_[2 * i + 1][j]);
                } else {
                    data_[i][j] = M::op(data_[i][2 * j], data_[i][2 * j + 1]); }
            }
        }
    }

    void set(int h, int w, T v) {
        assert(0 <= h && h < input_h_);
        assert(0 <= w && w < input_w_);
        h += size_h_;
        w += size_w_;
        data_[h][w] = v;

        for (int i = h; i > 0; i >>= 1) {
            for (int j = w; j > 0; j >>= 1) {
                if (i >= size_h_ && j >= size_w_) continue;

                if (log_table[size_h_] - log_table[i] == log_table[size_w_] - log_table[j]) {
                    data_[i][j] = M::op(M::op(data_[2 * i][2 * j], data_[2 * i + 1][2 * j + 1]),
                                        M::op(data_[2 * i + 1][2 * j], data_[2 * i][2 * j + 1]));
                } else if (log_table[size_h_] - log_table[i] > log_table[size_w_] - log_table[j]) {
                    data_[i][j] = M::op(data_[2 * i][j], data_[2 * i + 1][j]);
                } else {
                    data_[i][j] = M::op(data_[i][2 * j], data_[i][2 * j + 1]);
                }
            }
        }
    }

    T get(int h, int w) const {
        assert(0 <= h && h < input_h_);
        assert(0 <= w && w < input_w_);
        return data_[h + size_h_][w + size_w_];
    }

    T prod(int h1, int w1, int h2, int w2) const {
        assert(0 <= h1 && h1 <= h2 && h2 <= size_h_);
        assert(0 <= w1 && w1 <= w2 && w2 <= size_w_);

        T res = M::e();
        h1 += size_h_;
        w1 += size_w_;
        h2 += size_h_;
        w2 += size_w_;
        while (h1 < h2 && w1 < w2) {
            if (h1 & 1) {
                res = M::op(res, prod_row(h1, w1, w2));
                h1++;
            }
            if (h2 & 1) {
                h2--;
                res = M::op(res, prod_row(h2, w1, w2));
            }
            if (w1 & 1) {
                res = M::op(res, prod_column(h1, h2, w1));
                w1++;
            }
            if (w2 & 1) {
                w2--;
                res = M::op(res, prod_column(h1, h2, w2));
            }
            h1 >>= 1;
            w1 >>= 1;
            h2 >>= 1;
            w2 >>= 1;
        }
        return res;
    }

    T all_prod() const {
        return data_[1][1];
    }

  private:
    T prod_row(int i, int w1, int w2) const {
        assert(w1 <= w2);
        T res = M::e();
        while (w1 < w2) {
            if (w1 & 1) {
                res = M::op(res, data_[i][w1]);
                w1++;
            }
            if (w2 & 1) {
                w2--;
                res = M::op(res, data_[i][w2]);
            }
            w1 >>= 1;
            w2 >>= 1;
        }
        return res;
    }

    T prod_column(int h1, int h2, int j) const {
        assert(h1 <= h2);
        T res = M::e();
        while (h1 < h2) {
            if (h1 & 1) {
                res = M::op(res, data_[h1][j]);
                h1++;
            }
            if (h2 & 1) {
                h2--;
                res = M::op(res, data_[h2][j]);
            }
            h1 >>= 1;
            h2 >>= 1;
        }
        return res;
    }

    int input_h_;
    int input_w_;
    int size_h_;
    int size_w_;
    std::vector<std::vector<T>> data_;
    std::vector<unsigned> log_table;
};

}  //  namespace nono
