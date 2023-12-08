#pragma once

#include <functional>
#include <memory>
#include <numeric>
#include <type_traits>

#include "data-structure/dynamic-segment-tree.hpp"

namespace nono {

//  brief:
//  - dynamic-segment-tree-2d. 一点更新, 範囲取得
//  - 座圧せずに使えるがとても遅い
//
//  tparam:
//  - `T`: 配列の要素の型
//  - `op`: 演算関数. 戻り値 `T`, 引数 `T, T` でなければならない.
//  - `e`: 単位元を返す関数. 戻り値 `T`, 引数 `void` でなければならない.
//  - `S`: 添字の型
template <class T, auto op, auto e, class S = long long>
class DynamicSegmentTree2D {
    static_assert(std::is_convertible_v<decltype(op), std::function<T(T, T)>>
                  && std::is_convertible_v<decltype(e), std::function<T()>>);
    struct Node;
    using NodePtr = std::unique_ptr<Node>;
    using isize = S;

    //  brief:
    //  - 一次元の領域を表す構造体
    //  - あまりにも引数が面倒になったので利用
    struct Bounds {
        isize lower;
        isize upper;

        Bounds() {}
        Bounds(isize lower, isize upper): lower(lower), upper(upper) {
            assert(lower <= upper);
        }

        bool inside(isize pos) const {
            return lower <= pos && pos < upper;
        }

        bool outside(isize pos) const {
            return !inside(pos);
        }

        bool intersect(Bounds other) const {
            return !(contain(other) || (other.upper <= lower) || (upper <= other.lower));
        }

        bool contain(Bounds other) const {
            return lower <= other.lower && other.upper <= upper;
        }

        isize median() const {
            assert(lower < upper);
            if ((lower >= 0) ^ (upper >= 0)) {
                return (lower + upper) / 2;
            } else {
                return lower + (upper - lower) / 2;
            }
        }
    };

    struct Node {
        Node(): left(nullptr), right(nullptr) {}
        Node(isize lb, isize ub): segtree(lb, ub), left(nullptr), right(nullptr) {}

        void update(isize w) {
            T value = e();
            if (left) {
                value = op(value, left->segtree.get(w));
            }
            if (right) {
                value = op(value, right->segtree.get(w));
            }
            segtree.set(w, value);
        }

        DynamicSegmentTree<T, op, e> segtree;
        NodePtr left;
        NodePtr right;
    };

  public:
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(1)
    //
    //  param:
    //  - `{h/w}_lb`: 格納する領域の下界
    //  - `{h/w}_ub`: 格納する領域の上界
    DynamicSegmentTree2D(isize h_lb = std::numeric_limits<isize>::min(), isize h_ub = std::numeric_limits<isize>::max(),
                         isize w_lb = std::numeric_limits<isize>::min(), isize w_ub = std::numeric_limits<isize>::max())
        : root_(nullptr),
          h_(h_lb, h_ub),
          w_(w_lb, w_ub) {}

    //  complexity:
    //  - O((logN) ^ 2)
    void set(isize pos_h, isize pos_w, T value) {
        assert(h_.inside(pos_h) && w_.inside(pos_w));
        set(root_, h_, pos_h, pos_w, value);
    }

    //  complexity:
    //  - O((logN) ^ 2)
    T get(isize pos_h, isize pos_w) {
        assert(h_.inside(pos_h) && w_.inside(pos_w));
        return get(root_, h_, pos_h, pos_w);
    }

    //  complexity:
    //  - O((logN) ^ 2)
    T prod(isize pos_h1, isize pos_w1, isize pos_h2, isize pos_w2) {
        assert(h_.inside(pos_h1) && w_.inside(pos_w1));
        assert(h_.inside(pos_h2) && w_.inside(pos_w2));
        assert(pos_h1 <= pos_h2);
        assert(pos_w1 <= pos_w2);
        return prod(root_, h_, Bounds(pos_h1, pos_h2), Bounds(pos_w1, pos_w2));
    }

    T all_prod() {
        return root_ ? root_->segtree.all_prod() : e();
    }

  private:
    void set(NodePtr& node, Bounds h, isize pos_h, isize pos_w, T value) {
        assert(h.inside(pos_h));
        if (!node) {
            node = std::make_unique<Node>(w_.lower, w_.upper);
        }
        if (h.upper == h.lower + 1) {
            node->segtree.set(pos_w, value);
            return;
        }
        isize med = h.median();
        if (h.lower <= pos_h && pos_h < med) {
            set(node->left, Bounds(h.lower, med), pos_h, pos_w, value);
        } else {
            assert(med <= pos_h && pos_h < h.upper);
            set(node->right, Bounds(med, h.upper), pos_h, pos_w, value);
        }
        node->update(pos_w);
    }

    T get(NodePtr& node, Bounds h, isize pos_h, isize pos_w) {
        assert(h.inside(pos_h));
        if (!node) {
            return e();
        }
        if (h.upper == h.lower + 1) {
            return node->segtree.get(pos_w);
        }
        isize med = h.median();
        if (h.lower <= pos_h && pos_h < med) {
            return get(node->left, Bounds(h.lower, med), pos_h, pos_w);
        } else {
            assert(med <= pos_h && pos_h < h.upper);
            return get(node->right, Bounds(med, h.upper), pos_h, pos_w);
        }
    }

    T prod(NodePtr& node, Bounds h, Bounds target_h, Bounds target_w) {
        if (!node) {
            return e();
        }
        if (target_h.contain(h)) {
            assert(target_h.lower <= h.lower && h.upper <= target_h.upper);
            return node->segtree.prod(target_w.lower, target_w.upper);
        } else if (target_h.intersect(h)) {
            isize med = h.median();
            return op(prod(node->left, Bounds(h.lower, med), target_h, target_w),
                      prod(node->right, Bounds(med, h.upper), target_h, target_w));
        } else {
            assert(target_h.upper <= h.lower || h.upper <= target_h.lower);
            return e();
        }
    }

    NodePtr root_;
    Bounds h_;
    Bounds w_;
};

}  //  namespace nono
