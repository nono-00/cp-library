#pragma once

#include <cassert>
#include <functional>
#include <limits>
#include <memory>
#include <type_traits>

namespace nono {

//  brief:
//  - dynamic-segment-tree. 一点更新, 範囲取得
//  - 座圧せずに使えるがとても遅い
//
//  tparam:
//  - `T`: 配列の要素の型
//  - `op`: 演算関数. 戻り値 `T`, 引数 `T, T` でなければならない.
//  - `e`: 単位元を返す関数. 戻り値 `T`, 引数 `void` でなければならない.
//  - `S`: 添字の型
template <class T, auto op, auto e, class S = long long>
class DynamicSegmentTree {
    static_assert(std::is_convertible_v<decltype(op), std::function<T(T, T)>>
                  && std::is_convertible_v<decltype(e), std::function<T()>>);
    struct Node;
    using NodePtr = std::unique_ptr<Node>;
    using isize = S;

    struct Node {
        Node(): left(nullptr), right(nullptr) {
            value = e();
        }
        Node(T value): value(value), left(nullptr), right(nullptr) {}

        void update() {
            value = e();
            if (left) {
                value = op(value, left->value);
            }
            if (right) {
                value = op(value, right->value);
            }
        }

        T value;
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
    //  - `lb`: 格納する領域の下界
    //  - `ub`: 格納する領域の上界
    DynamicSegmentTree(isize lb = std::numeric_limits<isize>::min(), isize ub = std::numeric_limits<isize>::max())
        : root_(nullptr),
          lb_(lb),
          ub_(ub) {
        assert(lb_ < ub_);
    }

    //  complexity:
    //  - O(log (ub - lb))
    void set(isize pos, T value) {
        assert(lb_ <= pos && pos < ub_);
        set(root_, lb_, ub_, pos, value);
    }

    //  complexity:
    //  - O(log (ub - lb))
    T get(isize pos) {
        assert(lb_ <= pos && pos < ub_);
        return get(root_, lb_, ub_, pos);
    }

    //  complexity:
    //  - O(log (ub - lb))
    T prod(isize lb, isize ub) {
        assert(lb_ <= lb && lb <= ub && ub <= ub_);
        return prod(root_, lb_, ub_, lb, ub);
    }

    //  complexity:
    //  - O(1)
    T all_prod() {
        return root_ ? root_->value : e();
    }

  private:
    //  note:
    //  - オーバーフロー回避のための関数
    inline isize median(isize lb, isize ub) {
        assert(lb < ub);
        if ((lb >= 0) ^ (ub >= 0)) {
            return (lb + ub) / 2;
        } else {
            return lb + (ub - lb) / 2;
        }
    }

    void set(NodePtr& node, isize lb, isize ub, isize pos, T value) {
        assert(lb <= pos && pos < ub);
        if (!node) {
            node = std::make_unique<Node>();
        }
        if (ub == lb + 1) {
            assert(pos == lb);
            node->value = value;
            return;
        }
        isize m = median(lb, ub);
        if (lb <= pos && pos < m) {
            set(node->left, lb, m, pos, value);
        } else {
            assert(m <= pos && pos < ub);
            set(node->right, m, ub, pos, value);
        }
        node->update();
    }

    T get(NodePtr& node, isize lb, isize ub, isize pos) {
        assert(lb <= pos && pos < ub);
        if (!node) {
            return e();
        }
        if (ub == lb + 1) {
            assert(pos == lb);
            return node->value;
        }
        isize m = median(lb, ub);
        if (lb <= pos && pos < m) {
            return get(node->left, lb, m, pos);
        } else {
            assert(m <= pos && pos < ub);
            return get(node->right, m, ub, pos);
        }
    }

    T prod(NodePtr& node, isize lb, isize ub, isize target_lb, isize target_ub) {
        if (!node || target_ub <= lb || ub <= target_lb) {
            return e();
        } else if (target_lb <= lb && ub <= target_ub) {
            return node->value;
        } else {
            isize m = median(lb, ub);
            return op(prod(node->left, lb, m, target_lb, target_ub), prod(node->right, m, ub, target_lb, target_ub));
        }
    }

    NodePtr root_;
    isize lb_;
    isize ub_;
};

}  //  namespace nono
