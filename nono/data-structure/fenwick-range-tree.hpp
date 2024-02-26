#pragma once

#include <vector>

#include "nono/data-structure/fenwick-tree.hpp"
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
template <class G, class U>
class FenwickRangeTree {
    using T = G::value_type;

  public:
    //  brief:
    //  - コンストラクタ
    //
    //  param:
    //  - `points`: 一点加算の座標
    //
    //  complexity:
    //  - O(N(logN)^2)
    FenwickRangeTree(const std::vector<std::pair<U, U>>& points) {
        std::vector<U> xs;
        for (auto [x, y]: points) {
            xs.push_back(x);
        }
        X = Compressor(std::move(xs));
        int n = X.size();
        std::vector<std::vector<U>> ys(n + 1);
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
    void apply(U x, U y, T elem) {
        int i = X.compress(x);
        for (i++; i <= X.size(); i += i & -i) {
            fens[i].apply(Ys[i].compress(y), elem);
        }
    }

    void set(U x, U y, T elem) {
        apply(x, y, G::op(elem, G::inv(get(x, y))));
    }

    //  brief:
    //  - 領域取得
    //
    //  complexity:
    //  - O((logN)^2)
    T prod(U x1, U y1, U x2, U y2) const {
        return G::op(prod(x2, y1, y2), G::inv(prod(x1, y1, y2)));
    }

    T get(U x, U y) const {
        return prod(x, y, x + 1, y + 1);
    }

  private:
    T prod(U x, U y1, U y2) const {
        T result = G::e();
        for (int i = X.compress(x); i > 0; i -= i & -i) {
            result = G::op(result, fens[i].prod(Ys[i].compress(y1), Ys[i].compress(y2)));
        }
        return result;
    }

    Compressor<U> X;
    std::vector<Compressor<U>> Ys;
    std::vector<FenwickTree<G>> fens;
};

}  //  namespace nono
