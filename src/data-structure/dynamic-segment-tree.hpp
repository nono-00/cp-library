#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <numeric>
#include <type_traits>

namespace nono {

template <class T, auto op, auto e>
class DynamicSegmentTree {
    static_assert(std::is_convertible_v<decltype(op), std::function<T(T, T)>>
                  && std::is_convertible_v<decltype(e), std::function<T()>>);
    struct Node;
    using NodePtr = std::unique_ptr<Node>;
    using isize = long long;

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
    DynamicSegmentTree(isize lb = std::numeric_limits<isize>::min(), isize ub = std::numeric_limits<isize>::max())
        : root(nullptr),
          lb_(lb),
          ub_(ub) {}

    void set(isize pos, T value) {
        set(root, lb_, ub_, pos, value);
    }

    T get(isize pos) {
        return get(root, lb_, ub_, pos);
    }

    T prod(isize lb, isize ub) {
        return prod(root, lb_, ub_, lb, ub);
    }

    T prod() {
        return root ? root->value : e();
    }

  private:
    inline isize median(isize lb, isize ub) {
        assert(ub > lb);
        if ((lb >= 0) ^ (ub >= 0)) {
            return (lb + ub) / 2;
        } else {
            return lb + (ub - lb) / 2;
        }
    }

    void set(NodePtr& node, isize lb, isize ub, isize pos, T value) {
        if (!node) {
            node = std::make_unique<Node>();
        }
        if (ub == lb + 1) {
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
        if (!node) {
            node = std::make_unique<Node>();
        }
        if (ub == lb + 1) {
            return node->value;
        }
        isize m = median(lb, ub);
        if (lb <= pos && pos < m) {
            return get(node->left, lb, m, pos);
        } else {
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

    NodePtr root;
    isize lb_;
    isize ub_;
};

}  //  namespace nono
