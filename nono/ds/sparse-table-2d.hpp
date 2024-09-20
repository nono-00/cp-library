#pragma once

#include <bit>
#include <cassert>
#include <vector>

#include "nono/ds/sparse-table.hpp"

namespace nono {

///  brief : 静的な矩形領域積クエリを前処理\\( O(n \log ^ 2 n) \\), クエリ \\( O(1) \\)で. メモリ使用量がバカ.
template <class M>
class SparseTable2D {
    using T = M::Value;

  public:
    SparseTable2D() {}
    SparseTable2D(std::vector<std::vector<T>> data): h_(data.size()), w_(data.front().size()) {
        const int log = std::bit_width(static_cast<unsigned>(h_));
        std::vector<std::vector<std::vector<T>>> temp(log);
        temp.front().resize(h_);
        for (int i = 0; i < h_; i++) {
            temp[0][i] = std::move(data[i]);
        }
        for (int i = 1; i < log; i++) {
            int length = 1 << i;
            int half = length >> 1;
            temp[i].assign(h_ - length + 1, std::vector<T>(w_));
            for (int j = 0; j + length <= h_; j++) {
                const auto& lhs = temp[i - 1][j];
                const auto& rhs = temp[i - 1][j + half];
                auto& tar = temp[i][j];
                for (int k = 0; k < w_; k++) {
                    tar[k] = M::op(lhs[k], rhs[k]);
                }
            }
        }
        data_.resize(log);
        for (int i = 0; i < log; i++) {
            int length = 1 << i;
            data_[i].resize(h_ - length + 1);
            for (int j = 0; j + length <= h_; j++) {
                data_[i][j] = SparseTable<M>(std::move(temp[i][j]));
            }
        }
    }

    T prod(int h1, int w1, int h2, int w2) const {
        assert(0 <= h1 && h1 <= h2 && h2 <= h_);
        assert(0 <= w1 && w1 <= w2 && w2 <= w_);
        if (h1 == h2) return M::e();
        const int log = std::bit_width(static_cast<unsigned>(h2 - h1)) - 1;
        return M::op(data_[log][h1].prod(w1, w2), data_[log][h2 - (1 << log)].prod(w1, w2));
    }

  private:
    int h_;
    int w_;
    //  data[width][先頭 h']:
    std::vector<std::vector<SparseTable<M>>> data_;
};

}  //  namespace nono
