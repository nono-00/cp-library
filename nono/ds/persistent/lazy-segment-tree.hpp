#pragma once

#include <cassert>
#include <memory>
#include <numeric>

namespace nono {

namespace persistent {

namespace lazy_segment_tree_internal {

template <class M>
struct Node;

template <class M>
using NodePtr = std::shared_ptr<Node<M>>;

template <class M>
struct Node {
    Node(): total(M::e()) {}
    M::Value total;
    M::Act act;
    NodePtr<M> left = nullptr;
    NodePtr<M> right = nullptr;
};

template <class M>
M::Value prod(NodePtr<M> node) {
    return node ? node->total : M::e();
}

template <class M>
[[nodiscard]] NodePtr<M> apply(NodePtr<M> node, typename M::Act act) {
    if (!node) return nullptr;
    node = std::make_shared<Node<M>>(*node);
    node->total = M::mapping(act, node->total);
    node->act = M::composition(act, node->act);
    return node;
}

template <class M>
void update(NodePtr<M> node) {
    node->total = M::op(prod(node->left), prod(node->right));
}

template <class M>
void push(NodePtr<M> node) {
    if (!node) return;
    node->left = apply(node->left, node->act);
    node->right = apply(node->right, node->act);
    node->act = M::id();
}

template <class M>
[[nodiscard]] NodePtr<M> set(NodePtr<M> node, int lb, int ub, int pos, typename M::Value element) {
    assert(lb <= pos && pos < ub);
    if (!node) {
        node = std::make_shared<Node<M>>();
    } else {
        node = std::make_shared<Node<M>>(*node);
    }
    push(node);
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
[[nodiscard]] NodePtr<M> apply(NodePtr<M> node, int lb, int ub, int query_l, int query_r, typename M::Act act) {
    if (query_r <= lb || ub <= query_l) {
        return node;
    }
    if (!node) {
        node = std::make_shared<Node<M>>();
    } else {
        node = std::make_shared<Node<M>>(*node);
    }
    push(node);
    if (query_l <= lb && ub <= query_r) {
        return apply(node, act);
    } else {
        int m = std::midpoint(lb, ub);
        node->left = apply(node->left, lb, m, query_l, query_r, act);
        node->right = apply(node->right, m, ub, query_l, query_r, act);
        update(node);
        return node;
    }
}

template <class M>
M::Value prod(NodePtr<M> node, int lb, int ub, int query_l, int query_r) {
    if (!node || query_r <= lb || ub <= query_l) {
        return M::e();
    }
    if (query_l <= lb && ub <= query_r) {
        return node->total;
    } else {
        push(node);
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
        push(output_node);
        push(input_node);
        int m = std::midpoint(lb, ub);
        output_node->left = copy(output_node->left, lb, m, query_l, query_r, input_node->left);
        output_node->right = copy(output_node->right, m, ub, query_l, query_r, input_node->right);
        update(output_node);
        return output_node;
    }
}

};  //  namespace lazy_segment_tree_internal

//  # LazySegmentTree
//  永続遅延セグメント木。set / apply / copy は自分を書き換えず、新しい木を返す。古い版もそのまま使える
//  Mはact-monoid。値を設定していない位置は M::e() として扱う
template <class M>
class LazySegmentTree {
    using T = M::Value;
    using F = M::Act;

  public:
    //  # LazySegmentTree(n)
    //  長さnで、全要素が M::e() の木を作る
    //  O(1)
    explicit LazySegmentTree(int n = 0): n_(n), root_(nullptr) {}
    [[nodiscard]] LazySegmentTree(int n, lazy_segment_tree_internal::NodePtr<M> root): n_(n), root_(root) {}

    //  # set(p, element)
    //  data[p] を element にした木を返す
    //  0 <= p < n でないと壊れる
    //  O(log n)
    [[nodiscard]] LazySegmentTree set(int p, T element) const {
        return LazySegmentTree(n_, lazy_segment_tree_internal::set(root_, 0, n_, p, element));
    }

    //  # get(p)
    //  return data[p]
    //  O(log n)
    T get(int p) const {
        return prod(p, p + 1);
    }

    //  # prod(left, right)
    //  return op[for i in [left, right)](data[i])
    //  O(log n)
    T prod(int left, int right) const {
        return lazy_segment_tree_internal::prod(root_, 0, n_, left, right);
    }

    //  # all_prod()
    //  return op[for i in [0, n)](data[i])
    //  O(1)
    T all_prod() const {
        return lazy_segment_tree_internal::prod(root_);
    }

    //  # apply(p, act)
    //  data[p] に act を作用させた木を返す
    //  O(log n)
    [[nodiscard]] LazySegmentTree apply(int p, F act) const {
        return apply(p, p + 1, act);
    }

    //  # apply(left, right, act)
    //  [for i in [left, right)](data[i] <= mapping(act, data[i])) とした木を返す
    //  O(log n)
    [[nodiscard]] LazySegmentTree apply(int left, int right, F act) const {
        return LazySegmentTree(n_, lazy_segment_tree_internal::apply(root_, 0, n_, left, right, act));
    }

    //  # copy(left, right, segtree)
    //  [left, right) の範囲だけを、segtreeの内容に置き換えた木を返す
    //  segtreeと長さが違うと壊れる
    //  segtreeの側に、まだ値を設定していない部分があると壊れる(未設定の節を辿ってしまう)
    //  O(log n)
    [[nodiscard]] LazySegmentTree copy(int left, int right, LazySegmentTree segtree) const {
        assert(n_ == segtree.n_);
        return LazySegmentTree(n_, lazy_segment_tree_internal::copy(root_, 0, n_, left, right, segtree.root_));
    }

  private:
    int n_;
    lazy_segment_tree_internal::NodePtr<M> root_;
};

}  //  namespace persistent

}  //  namespace nono
