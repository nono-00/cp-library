#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace nono {

///  # Sparse Table
///  静的な区間積クエリを前処理 O(nlogn), クエリ O(1)で.
///  だいたいDisjointを使った方が良い
template <class M>
class SparseTable {
    using T = M::Value;

  public:
    SparseTable() {}
    SparseTable(std::vector<T> data): n_(data.size()) {
        const int log = std::bit_width((unsigned)n_);
        data_.resize(log);
        data_[0] = std::move(data);
        for (int i = 1; i < log; i++) {
            int length = 1 << i;
            int half = length >> 1;
            data_[i].resize(n_ - length + 1);
            for (int j = 0; j + length <= n_; j++) {
                data_[i][j] = M::op(data_[i - 1][j], data_[i - 1][j + half]);
            }
        }
    }

    ///  # prod(l, r)
    ///  return op[for i in [l, r)](data[i])
    ///  O(logn)
    T prod(int left, int right) const {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        if (left == right) return M::e();
        const int log = 31 - std::countl_zero(static_cast<unsigned>(right - left));
        return M::op(data_[log][left], data_[log][right - (1 << log)]);
    }

  private:
    int n_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
