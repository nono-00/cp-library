#pragma once

#include <cassert>
#include <ranges>
#include <utility>
#include <vector>

#include "data-structure/segment-tree.hpp"
#include "utility/compressor.hpp"

namespace nono {

template <class T, auto op, auto e, class S>
class RangeSegmentTree {
  public:
    //  brief:
    //  - コンストラクタ
    //
    //  param:
    //  - `points`: 一点加算の座標
    //
    //  complexity:
    //  - O(N(logN)^2)
    RangeSegmentTree(const std::vector<std::pair<S, S>>& points): segtrees(1), Ys(1) {
        std::vector<S> xs;
        for (auto [x, y]: points) {
            xs.push_back(x);
        }
        X = Compressor(std::move(xs));
        int n = X.size();
        std::vector<std::vector<S>> ys(2 * n);
        Ys.reserve(2 * n);
        segtrees.reserve(2 * n);

        for (int i = 0; i < std::ssize(points); i++) {
            auto [x, y] = points[i];
            int j = X.compress(x);
            ys[j + n].push_back(y);
        }
        for (int i = n - 1; i > 0; i--) {
            std::ranges::copy(ys[2 * i], std::back_inserter(ys[i]));
            std::ranges::copy(ys[2 * i + 1], std::back_inserter(ys[i]));
        }
        for (int i = 1; i < 2 * n; i++) {
            Ys.emplace_back(std::move(ys[i]));
            segtrees.emplace_back(Ys[i].size());
        }
    }

    //  brief:
    //  - 一点更新
    //
    //  complexity:
    //  - O((logN)^2)
    void set(S x, S y, T w) {
        assert(X.contains(x));
        int i = X.compress(x);
        i += X.size();
        assert(Ys[i].contains(y));
        segtrees[i].set(Ys[i].compress(y), w);
        for (i >>= 1; i > 0; i >>= 1) {
            assert(Ys[i].contains(y));
            T val = e();
            if (Ys[2 * i].contains(y)) {
                val = op(val, segtrees[2 * i].get(Ys[2 * i].compress(y)));
            }
            if (Ys[2 * i + 1].contains(y)) {
                val = op(val, segtrees[2 * i + 1].get(Ys[2 * i + 1].compress(y)));
            }
            segtrees[i].set(Ys[i].compress(y), val);
        }
    }

    //  brief:
    //  - 一点取得
    //
    //  complexity:
    //  - O(logN);
    T get(S x, S y) {
        assert(X.contains(x));
        int i = X.compress(x);
        i += X.size();
        assert(Ys[i].contains(y));
        return segtrees[i].get(Ys[i].compress(y));
    }

    //  brief:
    //  - 領域取得
    //
    //  complexity:
    //  - O((logN)^2)
    T prod(S x1, S y1, S x2, S y2) {
        int left = X.compress(x1);
        int right = X.compress(x2);
        T res = e();
        for (left += X.size(), right += X.size(); left < right; left >>= 1, right >>= 1) {
            if (left & 1) {
                res = op(res, segtrees[left].prod(Ys[left].compress(y1), Ys[left].compress(y2)));
                left++;
            }
            if (right & 1) {
                right--;
                res = op(res, segtrees[right].prod(Ys[right].compress(y1), Ys[right].compress(y2)));
            }
        }
        return res;
    }

    T all_prod() {
        return segtrees[1].all_prod();
    }

  private:
    Compressor<S> X;
    std::vector<Compressor<S>> Ys;
    std::vector<SegmentTree<T, op, e>> segtrees;
};

}  //  namespace nono
