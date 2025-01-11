#pragma once

#include <cassert>
#include <random>
#include <utility>

#include "../structure/act-monoid.hpp"

namespace nono {

namespace lazy_treap_node {

///  # Treap Node
///  根のpが最も小さい
template <class M>
struct Node {
    static std::mt19937 rng;
    using Rev = act_monoid::Rev<M>;
    using T = M::Value;

    bool rev = false;
    Rev::Value elem, tot;
    Rev::Act act;

    unsigned priority;
    int size;
    Node* l = nullptr;
    Node* r = nullptr;

    ///  遅延セグ木と違い, 今のrev, actはまだ適用していない
    Node(): elem(M::e()), tot(M::e()), act(M::id()), priority(rng()), size(1) {}
    Node(T elem): elem(elem), tot(elem), act(M::id()), priority(rng()), size(1) {}
    ~Node() {
        if (l) delete l;
        if (r) delete r;
    }
    void push() {
        if (rev) {
            std::swap(l, r);
            std::swap(tot.ord, tot.rev);
            if (l) l->rev ^= true;
            if (r) r->rev ^= true;
            rev = false;
        }
        if (act != Rev::id()) {
            elem = Rev::mapping(act, elem);
            tot = Rev::mapping(act, tot);
            if (l) l->act = Rev::composition(act, l->act);
            if (r) r->act = Rev::composition(act, r->act);
            act = Rev::id();
        }
    }
    void update() {
        tot = M::e();
        size = 1;
        if (l) {
            tot = Rev::op(tot, l->tot);
            size += l->size;
        }
        tot = Rev::op(tot, elem);
        if (r) {
            tot = Rev::op(tot, r->tot);
            size += r->size;
        }
    }
};

template <class M>
std::mt19937 Node<M>::rng{std::random_device{}()};

template <class M>
using NodePtr = Node<M>*;

///  # split(node, k)
///  return {node [0, k), node [k, n)}
///  O(log n)
template <class M>
std::pair<NodePtr<M>, NodePtr<M>> split(NodePtr<M> node, int k) {
    if (node) node->push();
    if (k == 0) return {nullptr, node};
    if (node->size == k) return {node, nullptr};
    if (!node->l || node->l->size < k) {
        auto [lhs, rhs] = split(node->r, k - 1 - (node->l ? node->l->size : 0));
        node->r = lhs;
        if (node->l) node->l->push();
        node->update();
        return {node, rhs};
    } else {
        auto [lhs, rhs] = split(node->l, k);
        node->l = rhs;
        if (node->r) node->r->push();
        node->update();
        return {lhs, node};
    }
}

///  # merge(lhs, rhs)
///  return lhs + rhs
///  O(log n)
template <class M>
NodePtr<M> merge(NodePtr<M> lhs, NodePtr<M> rhs) {
    if (lhs) lhs->push();
    if (rhs) rhs->push();
    if (!lhs) return rhs;
    if (!rhs) return lhs;
    if (lhs->priority < rhs->priority) {
        lhs->r = merge(lhs->r, rhs);
        if (lhs->l) lhs->l->push();
        lhs->update();
        return lhs;
    } else {
        rhs->l = merge(lhs, rhs->l);
        if (rhs->r) rhs->r->push();
        rhs->update();
        return rhs;
    }
}

///  # insert(node, k, v)
///  return node[0, k) + {v} + node[k, n)
///  O(n)
template <class M>
NodePtr<M> insert(NodePtr<M> node, int k, typename M::Value v) {
    auto [lhs, rhs] = split(node, k);
    return merge(merge(lhs, new Node<M>(v)), rhs);
}

///  # erase(node, k)
///  return node[0, k - 1) + node[k, n)
///  O(n)
template <class M>
NodePtr<M> erase(NodePtr<M> node, int k) {
    auto [lhs, t] = split(node, k);
    auto [mhs, rhs] = split(t, 1);
    delete mhs;
    return merge(lhs, rhs);
}

}  //  namespace lazy_treap_node

///  # LazyTreap
///  平衡二分木
///  insertやerase, reverse, prod, applyなどが大体O(logn)
template <class M>
class LazyTreap {
    using T = M::Value;
    using F = M::Act;

  public:
    LazyTreap() {}
    ~LazyTreap() {
        if (root_) delete root_;
    }

    ///  # size()
    ///  要素数
    ///  O(1)
    int size() {
        return root_ ? root_->size : 0;
    }

    ///  # insert(k, v)
    ///  data <= data[0:k] + {v} + data[k:]
    ///  O(logn)
    void insert(int k, T v) {
        assert(0 <= k && k <= size());
        root_ = lazy_treap_node::insert(root_, k, v);
    }

    ///  # push_back(v)
    ///  data <= data + {v}
    ///  O(logn)
    void push_back(T v) {
        insert(size(), v);
    }

    ///  # erase(k)
    ///  data <= data[0:k - 1] + data[k:]
    ///  O(logn)
    void erase(int k) {
        assert(0 <= k && k < size());
        root_ = lazy_treap_node::erase(root_, k);
    }

    ///  # prod(l, r)
    ///  return op[for i in [l, r)](data[i])
    ///  O(logn)
    T prod(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return M::e();
        auto [t, rhs] = lazy_treap_node::split(root_, r);
        auto [lhs, mhs] = lazy_treap_node::split(t, l);
        T result = mhs->tot.ord;
        root_ = lazy_treap_node::merge(lazy_treap_node::merge(lhs, mhs), rhs);
        return result;
    }

    ///  # set(k, v)
    ///  data[k] <= v
    ///  O(logn)
    void set(int k, T v) {
        assert(0 <= k && k < size());
        auto [t, rhs] = lazy_treap_node::split(root_, k + 1);
        auto [lhs, mhs] = lazy_treap_node::split(t, k);
        mhs->elem = mhs->tot = v;
        root_ = lazy_treap_node::merge(lazy_treap_node::merge(lhs, mhs), rhs);
    }

    ///  # get(k)
    ///  return data[k]
    ///  O(logn)
    T get(int k) {
        assert(0 <= k && k < size());
        auto [t, rhs] = lazy_treap_node::split(root_, k + 1);
        auto [lhs, mhs] = lazy_treap_node::split(t, k);
        T result = mhs->v.ord;
        root_ = lazy_treap_node::merge(lazy_treap_node::merge(lhs, mhs), rhs);
        return result;
    }

    ///  # reverse(l, r)
    ///  data <= data[0:l] + reverse(data[l:r]) + data[r:]
    ///  O(logn)
    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [t, rhs] = lazy_treap_node::split(root_, r);
        auto [lhs, mhs] = lazy_treap_node::split(t, l);
        mhs->rev ^= true;
        root_ = lazy_treap_node::merge(lazy_treap_node::merge(lhs, mhs), rhs);
    }

    ///  # apply(l, r, f)
    ///  [for i in [l, r)](data[i] <= mapping(f, data[i]))
    ///  O(logn)
    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [t, rhs] = lazy_treap_node::split(root_, r);
        auto [lhs, mhs] = lazy_treap_node::split(t, l);
        mhs->act = M::composition(f, mhs->act);
        root_ = lazy_treap_node::merge(lazy_treap_node::merge(lhs, mhs), rhs);
    }

  private:
    lazy_treap_node::NodePtr<M> root_ = nullptr;
};

}  //  namespace nono