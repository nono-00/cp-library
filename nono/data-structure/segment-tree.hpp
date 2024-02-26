#pragma once

#include <cassert>
#include <vector>

namespace nono {

//  brief:
//  - segment-tree. 一点更新, 範囲取得
//
//  todo:
//  - セグ木上の二分探索
template <class M>
class SegmentTree {
    using T = M::value_type;

  public:
    SegmentTree() = default;
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(n)
    //
    //  param:
    //  - `size`: 元の配列のサイズ
    SegmentTree(int n): n_(n), data_(2 * n_, M::e()) {}

    //  brief:
    //  - コンストラクタ. `vector` で初期化する
    //
    //  complexity:
    //  - O(n)
    //
    //  param:
    //  - `data`: 初期化する配列
    SegmentTree(const std::vector<T>& data): n_(data.size()), data_(2 * n_, M::e()) {
        std::copy(data.begin(), data.end(), data_.begin() + n_);
        for (int i = n_ - 1; i > 0; i--) {
            data_[i] = M::op(data_[2 * i], data_[2 * i + 1]);
        }
    }

    //  brief:
    //  - `i` 番目の要素を `v` に変更する
    //
    //  complexity:
    //  - O(logn)
    void set(int i, T elem) {
        assert(0 <= i && i < n_);
        i += n_;
        data_[i] = elem;
        for (i >>= 1; i > 0; i >>= 1) {
            data_[i] = M::op(data_[2 * i], data_[2 * i + 1]);
        }
    }

    //  brief:
    //  - `i` 番目の要素を取得する
    //
    //  complexity:
    //  - O(1)
    T get(int i) const {
        assert(0 <= i && i < n_);
        return data_[i + n_];
    }

    //  brief:
    //  - `[l, r)` の積を取得する
    //
    //  complexity:
    //  - O(logn)
    //
    //  note:
    //  - 非可換でも可
    T prod(int left, int right) const {
        assert(0 <= left && left <= n_);
        assert(left <= right && right <= n_);
        T l_result = M::e();
        T r_result = M::e();
        for (left += n_, right += n_; left < right; left >>= 1, right >>= 1) {
            if (left & 1) l_result = M::op(l_result, data_[left++]);
            if (right & 1) r_result = M::op(data_[--right], r_result);
        }
        return M::op(l_result, r_result);
    }

  private:
    int n_;
    std::vector<T> data_;
};

}  //  namespace nono
