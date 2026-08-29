#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "../utility/compressor.hpp"
#include "./segment-tree.hpp"

namespace nono {

///  # 領域木segment tree盛り
///  一点変更座標が事前にわかる場合に使える二次元 segment tree.
template <class M, class Index>
class RangeSegmentTree {
    using T = M::Value;

  public:
    ///  # RangeSegmentTree(points)
    ///  points: 変更クエリが来る場所
    ///  O(N(logN)^2)
    RangeSegmentTree(const std::vector<std::pair<Index, Index>>& points): trees_(1), coord_y_(1) {
        std::vector<Index> xs;
        for (auto [x, y]: points) {
            xs.push_back(x);
        }
        coord_x_ = Compressor(std::move(xs));
        int n = coord_x_.size();
        std::vector<std::vector<Index>> ys(2 * n);
        coord_y_.reserve(2 * n);
        trees_.reserve(2 * n);

        for (int i = 0; i < std::ssize(points); i++) {
            auto [x, y] = points[i];
            int j = coord_x_.compress(x);
            ys[j + n].push_back(y);
        }
        for (int i = n - 1; i > 0; i--) {
            std::ranges::copy(ys[2 * i], std::back_inserter(ys[i]));
            std::ranges::copy(ys[2 * i + 1], std::back_inserter(ys[i]));
        }
        for (int i = 1; i < 2 * n; i++) {
            coord_y_.emplace_back(std::move(ys[i]));
            trees_.emplace_back(coord_y_[i].size());
        }
    }

    ///  # set(x, y, elem)
    ///  data[x][y] <= elem
    ///  O((logN)^2)
    void set(Index x, Index y, T w) {
        assert(coord_x_.contains(x));
        int i = coord_x_.compress(x);
        i += coord_x_.size();
        assert(coord_y_[i].contains(y));
        trees_[i].set(coord_y_[i].compress(y), w);
        for (i >>= 1; i > 0; i >>= 1) {
            assert(coord_y_[i].contains(y));
            T val = M::e();
            if (coord_y_[2 * i].contains(y)) {
                val = M::op(val, trees_[2 * i].get(coord_y_[2 * i].compress(y)));
            }
            if (coord_y_[2 * i + 1].contains(y)) {
                val = M::op(val, trees_[2 * i + 1].get(coord_y_[2 * i + 1].compress(y)));
            }
            trees_[i].set(coord_y_[i].compress(y), val);
        }
    }

    ///  # get(x, y)
    ///  return data[x][y]
    ///  O((logN)^2)
    T get(Index x, Index y) {
        assert(coord_x_.contains(x));
        int i = coord_x_.compress(x);
        i += coord_x_.size();
        assert(coord_y_[i].contains(y));
        return trees_[i].get(coord_y_[i].compress(y));
    }

    ///  # prod(x1, y1, x2, y2)
    ///  return op[for i in [x1, x2), for j in [y1, y2)](data[i][j])
    ///  O((logN)^2)
    T prod(Index x1, Index y1, Index x2, Index y2) {
        int left = coord_x_.compress(x1);
        int right = coord_x_.compress(x2);
        T res = M::e();
        for (left += coord_x_.size(), right += coord_x_.size(); left < right; left >>= 1, right >>= 1) {
            if (left & 1) {
                res = M::op(res, trees_[left].prod(coord_y_[left].compress(y1), coord_y_[left].compress(y2)));
                left++;
            }
            if (right & 1) {
                right--;
                res = M::op(res, trees_[right].prod(coord_y_[right].compress(y1), coord_y_[right].compress(y2)));
            }
        }
        return res;
    }

  private:
    Compressor<Index> coord_x_;
    std::vector<Compressor<Index>> coord_y_;
    std::vector<SegmentTree<M>> trees_;
};

}  //  namespace nono
