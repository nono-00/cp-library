#pragma once

#include <cassert>
#include <memory>
#include <numeric>

namespace nono {

namespace persistent {

namespace segment_tree {

template <class M>
struct Node;

template <class M>
using NodePtr = std::shared_ptr<Node<M>>;

template <class M>
struct Node {
    Node(): total(M::e()) {}
    M::Value total;
    NodePtr<M> left = nullptr;
    NodePtr<M> right = nullptr;
};

template <class M>
M::Value prod(NodePtr<M> node) {
    return node ? node->total : M::e();
}

template <class M>
void update(NodePtr<M> node) {
    node->total = M::op(prod(node->left), prod(node->right));
}

template <class M>
[[nodiscard]] NodePtr<M> set(NodePtr<M> node, int lb, int ub, int pos, typename M::Value element) {
    assert(lb <= pos && pos < ub);
    if (!node) {
        node = std::make_shared<Node<M>>();
    } else {
        node = std::make_shared<Node<M>>(*node);
    }
    if (ub == lb + 1) {
        assert(pos == lb);
        node->total = element;
        return node;
    }
    int m = std::midpoint(lb, ub);
    if (lb <= pos && pos < m) {
        node->left = set(node->left, lb, m, pos, element);
    } else {
        assert(m <= pos && pos < ub);
        node->right = set(node->right, m, ub, pos, element);
    }
    update(node);
    return node;
}

template <class M>
M::Value prod(NodePtr<M> node, int lb, int ub, int query_l, int query_r) {
    if (!node || query_r <= lb || ub <= query_l) {
        return M::e();
    }
    if (query_l <= lb && ub <= query_r) {
        return node->total;
    } else {
        int m = std::midpoint(lb, ub);
        return M::op(prod(node->left, lb, m, query_l, query_r), prod(node->right, m, ub, query_l, query_r));
    }
}

template <class M>
[[nodiscard]] NodePtr<M> copy(NodePtr<M> output_node, int lb, int ub, int query_l, int query_r, NodePtr<M> input_node) {
    if (query_r <= lb || ub <= query_l) {
        return output_node;
    } else if (query_l <= lb && ub <= query_r) {
        return input_node;
    } else {
        if (!output_node) {
            output_node = std::make_shared<Node<M>>();
        } else {
            output_node = std::make_shared<Node<M>>(*output_node);
        }
        int m = std::midpoint(lb, ub);
        output_node->left = copy(output_node->left, lb, m, query_l, query_r, input_node->left);
        output_node->right = copy(output_node->right, m, ub, query_l, query_r, input_node->right);
        update(output_node);
        return output_node;
    }
}

};  //  namespace segment_tree

template <class M>
class SegmentTree {
    using T = M::Value;

  public:
    explicit SegmentTree(int n = 0): n_(n), root_(nullptr) {}
    SegmentTree(int n, segment_tree::NodePtr<M> root): n_(n), root_(root) {}

    [[nodiscard]] SegmentTree set(int p, T element) const {
        return SegmentTree(n_, segment_tree::set(root_, 0, n_, p, element));
    }

    T get(int p) const {
        return prod(p, p + 1);
    }

    T prod(int left, int right) const {
        return segment_tree::prod(root_, 0, n_, left, right);
    }

    T all_prod() const {
        return segment_tree::prod(root_);
    }

    [[nodiscard]] SegmentTree copy(int left, int right, SegmentTree segtree) const {
        assert(n_ == segtree.n_);
        return SegmentTree(n_, segment_tree::copy(root_, 0, n_, left, right, segtree.root_));
    }

  private:
    int n_;
    segment_tree::NodePtr<M> root_;
};

}  //  namespace persistent

}  //  namespace nono
