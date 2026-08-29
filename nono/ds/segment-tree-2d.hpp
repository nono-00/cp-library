#pragma once

#include <cassert>
#include <vector>

#include "../ds/segment-tree.hpp"

namespace nono {

///  # 二次元SegmentTree
///  二次元領域和を扱うsegment tree
template <class M>
class SegmentTree2D {
    using T = M::Value;

  public:
    SegmentTree2D(int h, int w): h_(h), w_(w), trees_(2 * h_, SegmentTree<M>(w_)) {}
    SegmentTree2D(const std::vector<std::vector<T>>& data): h_(data.size()), w_(data.front().size()), trees_(2 * h_) {
        for (int i = 0; i < h_; i++) {
            trees_[i + h_] = SegmentTree<M>(data[i]);
        }
        for (int i = h_ - 1; i > 0; i--) {
            for (int j = 0; j < w_; j++) {
                trees_[i].set(j, trees_[2 * i].get(j), trees_[2 * i + 1].get(j));
            }
        }
    }
    ///  # set(i, j, elem)
    ///  data[h][w] <= elem
    ///  O(logh logw)
    void set(int i, int j, T elem) {
        assert(0 <= i && i < h_);
        assert(0 <= j && j < w_);
        for (trees_[i += h_].set(j, elem); i /= 2;) {
            trees_[i].set(j, M::op(trees_[2 * i].get(j), trees_[2 * i + 1].get(j)));
        }
    }

    ///  # get(i, j)
    ///  return data[i][j]
    ///  O(1)
    T get(int i, int j) const {
        assert(0 <= i && i < h_);
        assert(0 <= j && j < w_);
        return trees_[i + h_].get(j);
    }
    ///  # prod(i1, j1, i2, j2)
    ///  return op[for i in [i1, i2), for j in [j1, j2)](data[i][j])
    ///  O(logh logw)
    T prod(int i1, int j1, int i2, int j2) const {
        assert(0 <= i1 && i1 <= i2 && i2 <= h_);
        assert(0 <= j1 && j1 <= j2 && j2 <= w_);
        T result = M::e();
        for (i1 += h_, i2 += h_; i1 < i2; i1 /= 2, i2 /= 2) {
            if (i1 & 1) {
                result = M::op(result, trees_[i1++].prod(j1, j2));
            }
            if (i2 & 1) {
                result = M::op(result, trees_[--i2].prod(j1, j2));
            }
        }
        return result;
    }

    ///  # all_prod()
    ///  O(1)
    T all_prod() const {
        return trees_[1].all_prod();
    }

  private:
    int h_;
    int w_;
    std::vector<SegmentTree<M>> trees_;
};

}  //  namespace nono
