#pragma once

#include <utility>
#include <vector>

#include "../ds/fenwick-tree.hpp"
#include "../utility/compressor.hpp"

namespace nono {

///  # 領域木fenwick tree盛り.
///  一点加算座標が事前にわかる場合に使える二次元 fenwick tree.
template <class T, class Index>
class FenwickRangeTree {
  public:
    ///  # FenwickRangeTree(points)
    ///  points: 加算クエリが来る場所
    ///  O(N(logN)^2)
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

    ///  #add(x, y, elem)
    ///  data[x][y] <= data[x][y] + elem
    ///  O((logN)^2)
    void add(Index x, Index y, T elem) {
        int i = coord_x_.compress(x);
        for (i++; i <= coord_x_.size(); i += i & -i) {
            trees_[i].add(coord_y_[i].compress(y), elem);
        }
    }

    ///  #set(x, y, elem)
    ///  data[x][y] <= elem
    ///  O((logN)^2)
    void set(Index x, Index y, T elem) {
        add(x, y, elem - get(x, y));
    }

    ///  # sum
    ///  return sum[for i in [x1, x2), for j in [y1, y2)]
    ///  O((logN)^2)
    T sum(Index x1, Index y1, Index x2, Index y2) const {
        return sum(x2, y1, y2) - sum(x1, y1, y2);
    }

    ///  # get(x, y)
    ///  return data[x][y]
    ///  O((logN)^2)
    T get(Index x, Index y) const {
        return sum(x, y, x + 1, y + 1);
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
