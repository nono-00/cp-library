#pragma once

#include <cassert>
#include <limits>
#include <numeric>

namespace nono {

//  # DynamicSegmentTree
//  動的セグ木。座圧せずに使えるsegment tree. とても遅い.
//  Mはモノイド。設定していない位置の値は M::e()
template <class M, class Index = int>
class DynamicSegmentTree {
    using T = M::Value;
    struct Node;
    using NodePtr = Node*;

    struct Node {
        Node(): value(M::e()), left(nullptr), right(nullptr) {}
        Node(T value): value(value), left(nullptr), right(nullptr) {}
        ~Node() {
            delete left;
            delete right;
        }

        void update() {
            value = M::e();
            if (left) {
                value = M::op(value, left->value);
            }
            if (right) {
                value = M::op(value, right->value);
            }
        }

        T value;
        NodePtr left;
        NodePtr right;
    };

  public:
    //  # DynamicSegmentTree(lb, ub)
    //  [lb, ub)を扱うセグ木を作る
    //  lb < ub でないと壊れる
    //  O(1)
    DynamicSegmentTree(Index lb = std::numeric_limits<Index>::min(), Index ub = std::numeric_limits<Index>::max())
        : root_(nullptr),
          lb_(lb),
          ub_(ub) {
        assert(lb_ < ub_);
    }
    ~DynamicSegmentTree() {
        delete root_;
    }

    //  # set(pos, value)
    //  data[pos] <= value
    //  lb <= pos < ub でないと壊れる
    //  O(log (ub - lb))
    void set(Index pos, T value) {
        assert(lb_ <= pos && pos < ub_);
        set(root_, lb_, ub_, pos, value);
    }

    //  # get(pos)
    //  return data[pos]
    //  lb <= pos < ub でないと壊れる
    //  O(log (ub - lb))
    T get(Index pos) {
        assert(lb_ <= pos && pos < ub_);
        return get(root_, lb_, ub_, pos);
    }

    //  # prod(lb, ub)
    //  return op[for i in [lb, ub)](data[i])
    //  引数の区間が、コンストラクタに渡した区間に収まっていないと壊れる
    //  O(log (ub - lb))
    T prod(Index lb, Index ub) {
        assert(lb_ <= lb && lb <= ub && ub <= ub_);
        return prod(root_, lb_, ub_, lb, ub);
    }

    //  # all_prod()
    //  全体の積を返す
    //  O(1)
    T all_prod() {
        return root_ ? root_->value : M::e();
    }

  private:
    void set(NodePtr& node, Index lb, Index ub, Index pos, T value) {
        assert(lb <= pos && pos < ub);
        if (!node) {
            node = new Node();
        }
        if (ub == lb + 1) {
            assert(pos == lb);
            node->value = value;
            return;
        }
        Index m = std::midpoint(lb, ub);
        if (lb <= pos && pos < m) {
            set(node->left, lb, m, pos, value);
        } else {
            assert(m <= pos && pos < ub);
            set(node->right, m, ub, pos, value);
        }
        node->update();
    }

    T get(NodePtr& node, Index node_lb, Index node_rb, Index pos) {
        assert(node_lb <= pos && pos < node_rb);
        if (!node) {
            return M::e();
        }
        if (node_rb == node_lb + 1) {
            assert(pos == node_lb);
            return node->value;
        }
        Index m = std::midpoint(node_lb, node_rb);
        if (node_lb <= pos && pos < m) {
            return get(node->left, node_lb, m, pos);
        } else {
            assert(m <= pos && pos < node_rb);
            return get(node->right, m, node_rb, pos);
        }
    }

    T prod(NodePtr& node, Index lb, Index ub, Index target_lb, Index target_ub) {
        if (!node || target_ub <= lb || ub <= target_lb) {
            return M::e();
        } else if (target_lb <= lb && ub <= target_ub) {
            return node->value;
        } else {
            Index m = std::midpoint(lb, ub);
            return M::op(prod(node->left, lb, m, target_lb, target_ub), prod(node->right, m, ub, target_lb, target_ub));
        }
    }

    NodePtr root_;
    Index lb_;
    Index ub_;
};

}  //  namespace nono
