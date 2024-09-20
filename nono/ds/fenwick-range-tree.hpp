#pragma once

#include <vector>
#include <utility>

#include "nono/ds/fenwick-tree.hpp"
#include "nono/utility/compressor.hpp"

namespace nono {

//  brief:
//  - 領域木 fenwick treeのせ
//
//  details:
//  - 一点更新 領域取得が可能なFenwick Tree
//  - ただし、一点加算の座標をコンストラクタで与えなければならない
//
//  tparam:
//  - `T`: 値
//  - `U`, `V`: 座標
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
        X = Compressor(std::move(xs));
        int n = X.size();
        std::vector<std::vector<Index>> ys(n + 1);
        Ys.reserve(n + 1);

        for (auto [x, y]: points) {
            int i = X.compress(x);
            for (i++; i <= n; i += i & -i) {
                ys[i].push_back(y);
            }
        }

        for (int i = 0; i <= n; i++) {
            Ys.emplace_back(std::move(ys[i]));
            fens.emplace_back(Ys[i].size());
        }
    }

    //  brief:
    //  - 一点加算
    //
    //  complexity:
    //  - O((logN)^2)
    void apply(Index x, Index y, T elem) {
        int i = X.compress(x);
        for (i++; i <= X.size(); i += i & -i) {
            fens[i].apply(Ys[i].compress(y), elem);
        }
    }

    void set(Index x, Index y, T elem) {
        apply(x, y, G::op(elem, G::inv(get(x, y))));
    }

    //  brief:
    //  - 領域取得
    //
    //  complexity:
    //  - O((logN)^2)
    T prod(Index x1, Index y1, Index x2, Index y2) const {
        return G::op(prod(x2, y1, y2), G::inv(prod(x1, y1, y2)));
    }

    T get(Index x, Index y) const {
        return prod(x, y, x + 1, y + 1);
    }

  private:
    T prod(Index x, Index y1, Index y2) const {
        T result = G::e();
        for (int i = X.compress(x); i > 0; i -= i & -i) {
            result = G::op(result, fens[i].prod(Ys[i].compress(y1), Ys[i].compress(y2)));
        }
        return result;
    }

    Compressor<Index> X;
    std::vector<Compressor<Index>> Ys;
    std::vector<FenwickTree<T>> fens;
};

}  //  namespace nono
