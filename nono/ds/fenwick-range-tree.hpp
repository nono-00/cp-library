#pragma once

#include <utility>
#include <vector>

#include "../ds/fenwick-tree.hpp"
#include "../utility/compressor.hpp"

namespace nono {

///  brief : 領域木fenwick tree盛り. 一点加算座標が事前にわかる場合に使える二次元 fenwick tree.

//  brief:
//  - 領域木 fenwick treeのせ
//
//  details:
//  - 一点更新 領域取得が可能なFenwick Tree
//  - ただし、一点加算の座標をコンストラクタで与えなければならない
//
//  tparam:
//  - `T`: 値
//  - `Index`
//
//  depend:
//  - fenwick tree
//  - compressor
template <class T, class Index>
class FenwickRangeTree {
  public:
    //  brief:
    //  - コンストラクタ
    //
    //  param:
    //  - `points`: 一点加算の座標
    //
    //  complexity:
    //  - O(N(logN)^2)
    FenwickRangeTree(const std::vector<std::pair<Index, Index>>& points) {
        std::vector<Index> xs;
        for (auto [x, y]: points) {
            xs.push_back(x);
        }
        coord_x_ = Compressor(std::move(xs));
        int n = coord_x_.size();
        std::vector<std::vector<Index>> ys(n + 1);
        coord_y_.reserve(n + 1);

        for (auto [x, y]: points) {
            int i = coord_x_.compress(x);
            for (i++; i <= n; i += i & -i) {
                ys[i].push_back(y);
            }
        }

        for (int i = 0; i <= n; i++) {
            coord_y_.emplace_back(std::move(ys[i]));
            trees_.emplace_back(coord_y_[i].size());
        }
    }

    //  brief:
    //  - 一点加算
    //
    //  complexity:
    //  - O((logN)^2)
    void add(Index x, Index y, T elem) {
        int i = coord_x_.compress(x);
        for (i++; i <= coord_x_.size(); i += i & -i) {
            trees_[i].add(coord_y_[i].compress(y), elem);
        }
    }

    void set(Index x, Index y, T elem) {
        add(x, y, elem - get(x, y));
    }

    //  brief:
    //  - 領域取得
    //
    //  complexity:
    //  - O((logN)^2)
    T sum(Index x1, Index y1, Index x2, Index y2) const {
        return sum(x2, y1, y2) - sum(x1, y1, y2);
    }

    T get(Index x, Index y) const {
        return prod(x, y, x + 1, y + 1);
    }

  private:
    T sum(Index x, Index y1, Index y2) const {
        T result{0};
        for (int i = coord_x_.compress(x); i > 0; i -= i & -i) {
            result += trees_[i].sum(coord_y_[i].compress(y1), coord_y_[i].compress(y2));
        }
        return result;
    }

    Compressor<Index> coord_x_;
    std::vector<Compressor<Index>> coord_y_;
    std::vector<FenwickTree<T>> trees_;
};

}  //  namespace nono
