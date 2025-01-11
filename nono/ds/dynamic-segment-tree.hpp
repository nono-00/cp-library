#pragma once

#include <cassert>
#include <limits>
#include <numeric>

namespace nono {

///  # 動的セグ木
///  座圧せずに使えるsegment tree. とても遅い.
template <class M, class Index = int>
class DynamicSegmentTree {
    using T = M::Value;
    struct Node;
    using NodePtr = Node*;

    struct Node {
        Node(): left(nullptr), right(nullptr), value(M::e()) {}
        Node(T value): value(value), left(nullptr), right(nullptr) {}
        ~Node() {
            if (left) {
                delete left;
            }
            if (right) {
                delete right;
            }
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
    ///  # DynamicSegmentTree(lb, ub)
    ///  [lb, ub)を扱うセグ木を作る
    ///  O(1)
    DynamicSegmentTree(Index lb = std::numeric_limits<Index>::min(), Index ub = std::numeric_limits<Index>::max())
        : root_(nullptr),
          lb_(lb),
          ub_(ub) {
        assert(lb_ < ub_);
    }
    ~DynamicSegmentTree() {
        delete root_;
    }

    ///  # set(pos, value)
    ///  data[pos] <= value
    ///  O(log (ub - lb))
    void set(Index pos, T value) {
        assert(lb_ <= pos && pos < ub_);
        set(root_, lb_, ub_, pos, value);
    }

    ///  # get(pos)
    ///  return data[pos]
    ///  O(log (ub - lb))
    T get(Index pos) {
        assert(lb_ <= pos && pos < ub_);
        return get(root_, lb_, ub_, pos);
    }

    ///  # get(pos)
    ///  return prod[for i in [lb, ub)](data[i])
    ///  O(log (ub - lb))
    T prod(Index lb, Index ub) {
        assert(lb_ <= lb && lb <= ub && ub <= ub_);
        return prod(root_, lb_, ub_, lb, ub);
    }

    ///  # all_prod()
    ///  return prod[for i in [lb, ub)](data[i])
    ///  O(1)
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
