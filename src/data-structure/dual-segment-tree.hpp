#pragma once

#include <bit>
#include <cassert>
#include <concepts>
#include <functional>
#include <type_traits>
#include <vector>

namespace nono {


//  brief:
//  - segment-tree. 一点更新, 範囲取得
//
//  tparam:
//  - `T`: 配列の要素の型
//  - `e`: 単位元を返す関数. 戻り値 `T`, 引数 `void` でなければならない.
//  - 'F': 写像の型
//  - 'mapping': T mapping(F, T), 写像
//  - 'composition': F mapping(F, F) 合成写像
//
//  note:
//  - 作用が可換のときしか使えない
//  - そうじゃないなら遅延セグ木で
template <class T, auto e, class F, auto mapping, auto composition, auto id>
class DualSegmentTree {
    static_assert(std::is_convertible_v<decltype(e), std::function<T()>>
                  && std::is_convertible_v<decltype(mapping), std::function<F(F, T)>>
                  && std::is_convertible_v<decltype(composition), std::function<F(F, F)>>
                  && std::is_convertible_v<decltype(id), std::function<F()>>);

  public:
    DualSegmentTree() = default;
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(n)
    //
    //  param:
    //  - `size`: 元の配列のサイズ
    DualSegmentTree(int size)
        : input_size_(size),
          data_size_(std::bit_ceil(static_cast<unsigned>(input_size_))),
          data_(data_size_, e()),
          lazy_(data_size_, id()) {}

    //  brief:
    //  - コンストラクタ. `vector` で初期化する
    //
    //  complexity:
    //  - O(n)
    //
    //  param:
    //  - `data`: 初期化する配列
    DualSegmentTree(const std::vector<T>& data)
        : input_size_(data.size()),
          data_size_(std::bit_ceil(static_cast<unsigned>(input_size_))),
          data_(data_size_, e()),
          lazy_(data_size_, id()) {
        std::copy(data.begin(), data.end(), data_.begin());
    }

    //  brief:
    //  - pos番目をfで更新する
    //
    //  complexity:
    //  - O(1)
    void apply(int pos, F f) {
        assert(0 <= pos && pos < input_size_);
        data_[pos] = mapping(f, data_[pos]);
    }

    //  brief:
    //  - [l, r)をfで更新する
    //
    //  complexity:
    //  - O(logn)
    void apply(int l, int r, F f) {
        assert(0 <= l && l < input_size_);
        assert(l <= r && r <= input_size_);
        if (l & 1) {
            data_[l] = mapping(f, data_[l]);
            l++;
        }
        if (r & 1) {
            r--;
            data_[r] = mapping(f, data_[r]);
        }
        l = (l + data_size_) >> 1;
        r = (r + data_size_) >> 1;
        while (l < r) {
            if (l & 1) {
                lazy_[l] = composition(f, lazy_[l]);
                l++;
            }
            if (r & 1) {
                r--;
                lazy_[r] = composition(f, lazy_[r]);
            }
            l >>= 1;
            r >>= 1;
        }
    }

    //  brief:
    //  - pos番目の要素を取得する
    //
    //  complexity:
    //  - O(logn)
    T get(int pos) const {
        assert(0 <= pos && pos < input_size_);
        T res = data_[pos];
        pos = (pos + data_size_) >> 1;
        while (pos > 0) {
            res = mapping(lazy_[pos], res);
            pos >>= 1;
        }
        return res;
    }

  private:
    int input_size_;
    int data_size_;
    std::vector<T> data_;
    std::vector<F> lazy_;
};

}  //  namespace nono
