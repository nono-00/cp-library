#pragma once

#include <vector>

#include "data-structure/fenwick-tree.hpp"
#include "utility/compressor.hpp"

namespace nono {

// brief:
// - 領域木 fenwick treeのせ
//
// details:
// - 一点更新 領域取得が可能なFenwick Tree
// - ただし、一点加算の座標をコンストラクタで与えなければならない
//
// tparam:
// - `T`: 値
// - `U`, `V`: 座標
//
// depend:
// - fenwick tree
// - compressor
template <class T, class U, class V>
class RangeFenwickTree {
  public:
    // brief:
    // - コンストラクタ
    //
    // param:
    // - `points`: 一点加算の座標
    //
    // complexity:
    // - O(N(logN)^2)
    RangeFenwickTree(const std::vector<std::pair<U, V>>& points) {
        std::vector<U> xs;
        for (auto [x, y]: points) {
            xs.push_back(x);
        }
        X = Compressor(std::move(xs));
        int n = X.size();
        std::vector<std::vector<V>> ys(n + 1);
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

    // brief:
    // - 一点加算
    //
    // complexity:
    // - O((logN)^2)
    void add(U x, V y, T w) {
        int i = X.compress(x);
        for (i++; i <= X.size(); i += i & -i) {
            fens[i].add(Ys[i].compress(y), w);
        }
    }

    // brief:
    // - 領域取得
    //
    // complexity:
    // - O((logN)^2)
    T sum(U x1, V y1, U x2, V y2) {
        return sum(x2, y1, y2) - sum(x1, y1, y2);
    }

  private:
    T sum(U x, V y1, V y2) {
        T res{};
        for (int i = X.compress(x); i > 0; i -= i & -i) {
            res += fens[i].sum(Ys[i].compress(y1), Ys[i].compress(y2));
        }
        return res;
    }

    Compressor<U> X;
    std::vector<Compressor<V>> Ys;
    std::vector<FenwickTree<T>> fens;
};

}  //  namespace nono
