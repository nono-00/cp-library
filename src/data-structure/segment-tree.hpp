#pragma once

#include <bit>
#include <cassert>
#include <concepts>
#include <functional>
#include <limits>
#include <type_traits>
#include <vector>

namespace nono {

//  brief:
//  - segment-tree. 一点更新, 範囲取得
//
//  tparam:
//  - `T`: 配列の要素の型
//  - `op`: 演算関数. 戻り値 `T`, 引数 `T, T` でなければならない.
//  - `e`: 単位元を返す関数. 戻り値 `T`, 引数 `void` でなければならない.
//
//  note:
//  - 数値型 `T` に対しては, `MinSegmentTree<T>, MaxSegmentTree<T>`
//  - `SumSegmentTree<T>, MulSegmentTree<T>` が定義してある.
//
//  - template引数の制約が微妙.
//  - `SegmentTree<int, int op(double, long long), float e()>`
//  - みたいなものでもコンパイルできてしまう.
//
//  todo:
//  - セグ木上の二分探索
template <class T, auto op, auto e>
class SegmentTree {
    static_assert(std::is_convertible_v<decltype(op), std::function<T(T, T)>>
                  && std::is_convertible_v<decltype(e), std::function<T()>>);

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
    SegmentTree(int size)
        : input_size_(size),
          data_size_(std::bit_ceil(static_cast<unsigned>(input_size_))),
          data_(2 * data_size_, e()) {}

    //  brief:
    //  - コンストラクタ. `vector` で初期化する
    //
    //  complexity:
    //  - O(n)
    //
    //  param:
    //  - `data`: 初期化する配列
    SegmentTree(const std::vector<T>& data)
        : input_size_(data.size()),
          data_size_(std::bit_ceil(static_cast<unsigned>(input_size_))),
          data_(2 * data_size_, e()) {
        std::copy(data.begin(), data.end(), data_.begin() + data_size_);
        for (int i = data_size_ - 1; i > 0; i--) {
            data_[i] = op(data_[2 * i], data_[2 * i + 1]);
        }
    }

    //  brief:
    //  - `i` 番目の要素を `v` に変更する
    //
    //  complexity:
    //  - O(logn)
    void set(int i, const T& v) {
        assert(0 <= i && i < input_size_);
        i += data_size_;
        data_[i] = v;

        while (i > 1) {
            i /= 2;
            data_[i] = op(data_[2 * i], data_[2 * i + 1]);
        }
    }

    //  brief:
    //  - `i` 番目の要素を取得する
    //
    //  complexity:
    //  - O(1)
    T get(int i) const {
        assert(0 <= i && i < input_size_);
        return data_[i + data_size_];
    }

    //  brief:
    //  - `[l, r)` の積を取得する
    //
    //  complexity:
    //  - O(logn)
    //
    //  note:
    //  - 非可換でも可
    T prod(int l, int r) const {
        assert(0 <= l && l < input_size_);
        assert(l <= r && r <= input_size_);
        T l_result = e();
        T r_result = e();

        l += data_size_;
        r += data_size_;
        while (l < r) {
            if (l % 2 == 1) {
                l_result = op(l_result, data_[l]);
                l++;
            }
            if (r % 2 == 1) {
                r--;
                r_result = op(data_[r], r_result);
            }
            l /= 2;
            r /= 2;
        }

        return op(l_result, r_result);
    }

    //  brief:
    //  - [0, n) の積を取得する
    //
    //  complexity:
    //  - O(1)
    T all_prod() const {
        return data_[1];
    }

  private:
    int input_size_;
    int data_size_;
    std::vector<T> data_;
};

//  `SegmentTree` のaliasたち. 数値型のみに対応.
template <class T>
    requires std::is_arithmetic_v<T>
using MinSegmentTree = SegmentTree<T, [](const T& lhs, const T& rhs) { return (lhs < rhs ? lhs : rhs); },
                                   []() {
    return std::numeric_limits<T>::max();
}>;

template <class T>
    requires std::is_arithmetic_v<T>
using MaxSegmentTree = SegmentTree<T, [](const T& lhs, const T& rhs) { return (lhs > rhs ? lhs : rhs); },
                                   []() {
    return std::numeric_limits<T>::min();
}>;

template <class T>
    requires std::is_arithmetic_v<T>
using SumSegmentTree = SegmentTree<T, [](const T& lhs, const T& rhs) { return lhs + rhs; },
                                   []() {
    return static_cast<T>(0);
}>;

template <class T>
    requires std::is_arithmetic_v<T>
using MulSegmentTree = SegmentTree<T, [](const T& lhs, const T& rhs) { return lhs * rhs; },
                                   []() {
    return static_cast<T>(1);
}>;

}  //  namespace nono
