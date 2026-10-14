#pragma once

#include <cassert>
#include <limits>

#include "./dynamic-segment-tree.hpp"

namespace nono {

//  # DynamicSegmentTree2D
//  動的二次元セグ木。座圧せずに使える二次元segment tree.
//  とっっても遅い
//  Mはモノイド。opが可換でないと壊れる。設定していない位置の値は M::e()
//  以下の H, W は、コンストラクタに渡した区間の幅 (h_ub - h_lb, w_ub - w_lb)
template <class M, class Index = int>
class DynamicSegmentTree2D {
    using T = M::Value;
    struct Node;
    using NodePtr = Node*;

    //  # Bounds
    //  一次元の領域を表す構造体
    struct Bounds {
        Index lower;
        Index upper;

        Bounds() {}
        Bounds(Index lower, Index upper): lower(lower), upper(upper) {
            assert(lower <= upper);
        }

        bool inside(Index pos) const {
            return lower <= pos && pos < upper;
        }

        bool outside(Index pos) const {
            return !inside(pos);
        }

        bool intersect(Bounds other) const {
            return !(contain(other) || (other.upper <= lower) || (upper <= other.lower));
        }

        bool contain(Bounds other) const {
            return lower <= other.lower && other.upper <= upper;
        }

        Index median() const {
            assert(lower < upper);
            return std::midpoint(lower, upper);
        }
    };

    struct Node {
        Node(): left(nullptr), right(nullptr) {}
        Node(Index lb, Index ub): segtree(lb, ub), left(nullptr), right(nullptr) {}
        ~Node() {
            delete left;
            delete right;
        }

        void update(Index w) {
            T value = M::e();
            if (left) {
                value = M::op(value, left->segtree.get(w));
            }
            if (right) {
                value = M::op(value, right->segtree.get(w));
            }
            segtree.set(w, value);
        }

        DynamicSegmentTree<M, Index> segtree;
        NodePtr left;
        NodePtr right;
    };

  public:
    //  # DynamicSegmentTree2D(h_lb, h_ub, w_lb, w_ub)
    //  [h_lb, h_ub) x [w_lb, w_ub) を扱うセグ木を作る
    //  h_lb < h_ub, w_lb < w_ub でないと壊れる
    //  O(1)
    DynamicSegmentTree2D(Index h_lb = std::numeric_limits<Index>::min(), Index h_ub = std::numeric_limits<Index>::max(),
                         Index w_lb = std::numeric_limits<Index>::min(), Index w_ub = std::numeric_limits<Index>::max())
        : root_(nullptr),
          h_(h_lb, h_ub),
          w_(w_lb, w_ub) {}
    ~DynamicSegmentTree2D() {
        delete root_;
    }

    //  # set(pos_h, pos_w, value)
    //  data[pos_h][pos_w] <= value
    //  位置が範囲外だと壊れる
    //  O(log H log W)
    void set(Index pos_h, Index pos_w, T value) {
        assert(h_.inside(pos_h) && w_.inside(pos_w));
        set(root_, h_, pos_h, pos_w, value);
    }

    //  # get(pos_h, pos_w)
    //  return data[pos_h][pos_w]
    //  位置が範囲外だと壊れる
    //  O(log H log W)
    T get(Index pos_h, Index pos_w) {
        assert(h_.inside(pos_h) && w_.inside(pos_w));
        return get(root_, h_, pos_h, pos_w);
    }

    //  # prod(pos_h1, pos_w1, pos_h2, pos_w2)
    //  return op[for i in [pos_h1, pos_h2), for j in [pos_w1, pos_w2)](data[i][j])
    //  pos_h1 <= pos_h2, pos_w1 <= pos_w2 でないと壊れる。4つの位置が範囲外だと壊れる
    //  O(log H log W)
    T prod(Index pos_h1, Index pos_w1, Index pos_h2, Index pos_w2) {
        assert(h_.inside(pos_h1) && w_.inside(pos_w1));
        assert(h_.inside(pos_h2) && w_.inside(pos_w2));
        assert(pos_h1 <= pos_h2);
        assert(pos_w1 <= pos_w2);
        return prod(root_, h_, Bounds(pos_h1, pos_h2), Bounds(pos_w1, pos_w2));
    }

    //  # all_prod()
    //  全体の積を返す
    //  O(1)
    T all_prod() {
        return root_ ? root_->segtree.all_prod() : M::e();
    }

  private:
    void set(NodePtr& node, Bounds h, Index pos_h, Index pos_w, T value) {
        assert(h.inside(pos_h));
        if (!node) {
            node = new Node(w_.lower, w_.upper);
        }
        if (h.upper == h.lower + 1) {
            node->segtree.set(pos_w, value);
            return;
        }
        Index med = h.median();
        if (h.lower <= pos_h && pos_h < med) {
            set(node->left, Bounds(h.lower, med), pos_h, pos_w, value);
        } else {
            assert(med <= pos_h && pos_h < h.upper);
            set(node->right, Bounds(med, h.upper), pos_h, pos_w, value);
        }
        node->update(pos_w);
    }

    T get(NodePtr& node, Bounds h, Index pos_h, Index pos_w) {
        assert(h.inside(pos_h));
        if (!node) {
            return M::e();
        }
        if (h.upper == h.lower + 1) {
            return node->segtree.get(pos_w);
        }
        Index med = h.median();
        if (h.lower <= pos_h && pos_h < med) {
            return get(node->left, Bounds(h.lower, med), pos_h, pos_w);
        } else {
            assert(med <= pos_h && pos_h < h.upper);
            return get(node->right, Bounds(med, h.upper), pos_h, pos_w);
        }
    }

    T prod(NodePtr& node, Bounds h, Bounds target_h, Bounds target_w) {
        if (!node) {
            return M::e();
        }
        if (target_h.contain(h)) {
            assert(target_h.lower <= h.lower && h.upper <= target_h.upper);
            return node->segtree.prod(target_w.lower, target_w.upper);
        } else if (target_h.intersect(h)) {
            Index med = h.median();
            return M::op(prod(node->left, Bounds(h.lower, med), target_h, target_w),
                         prod(node->right, Bounds(med, h.upper), target_h, target_w));
        } else {
            assert(target_h.upper <= h.lower || h.upper <= target_h.lower);
            return M::e();
        }
    }

    NodePtr root_;
    Bounds h_;
    Bounds w_;
};

}  //  namespace nono
