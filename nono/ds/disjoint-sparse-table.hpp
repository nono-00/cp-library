#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace nono {

///  brief : 静的な区間積クエリを前処理\\( O(n \log n) \\), クエリ \\( O(1) \\)で. Disjointなので, 冪等律を要求しない.
template <class M>
class DisjointSparseTable {
    using T = M::Value;

  public:
    DisjointSparseTable(const std::vector<T>& data): n_(data.size()) {
        const int log = std::bit_width(static_cast<unsigned>(n_)) + 1;
        data_.assign(log, std::vector<T>(n_ + 1, M::e()));
        for (int i = 1; i < log; i++) {
            const int length = 1 << i;
            const int half = length >> 1;
            for (int mid = half; mid <= n_; mid += length) {
                const int left = mid - half;
                const int right = std::min(mid + half - 1, n_);
                for (int j = mid - 1; j >= left; j--) {
                    data_[i][j] = M::op(data[j], data_[i][j + 1]);
                }
                for (int j = mid; j < right; j++) {
                    data_[i][j + 1] = M::op(data_[i][j], data[j]);
                }
            }
        }
    }

    T prod(int left, int right) const {
        assert(0 <= left && left <= right);
        assert(left <= right && right <= n_);
        const int i = std::bit_width(static_cast<unsigned>(right ^ left));
        return M::op(data_[i][left], data_[i][right]);
    }

  private:
    int n_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
