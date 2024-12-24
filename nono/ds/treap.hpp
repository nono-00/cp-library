#pragma once

/// TODO : implementation

// #include "../monoid/monoid.hpp"
// #include "../template.hpp"
// #include "../xorshift.hpp"
// 根のpが最も小さい
namespace treap_node {
template <class M>
struct node {
    static xorshift<unsigned> rng;
    static void set_seed(unsigned seed) { rng.v = seed; }
    using T = M::T;
    using MM = monoid::with_rev<M>;
    using TT = MM::T;
    using nodeptr = node*;
    TT v, a;
    unsigned p;
    int s = 1;
    nodeptr l = nullptr, r = nullptr;
    // 遅延セグ木と違い, 今のrevはまだ適用していない
    bool rev = false;
    node(): v(MM::e()), a(MM::e()), p(rng()) {}
    node(TT v): v(v), a(v), p(rng()) {}
    ~node() {
        if (l) delete l;
        if (r) delete r;
    }
    void push() {
        if (rev) {
            swap(l, r);
            swap(a.ord, a.rev);
            if (l) l->rev ^= true;
            if (r) r->rev ^= true;
            rev = false;
        }
    }
    void update() {
        a = MM::e();
        s = 1;
        if (l) a = MM::op(a, l->a), s += l->s;
        a = MM::op(a, v);
        if (r) a = MM::op(a, r->a), s += r->s;
    }
};
template <class M>
xorshift<unsigned> node<M>::rng{random_device()()};
template <class M>
using nodeptr = node<M>*;
template <class M>
pair<nodeptr<M>, nodeptr<M>> split(nodeptr<M> p, int k) {
    if (p) p->push();
    if (k == 0) return {nullptr, p};
    if (p->s == k) return {p, nullptr};
    if (!p->l || p->l->s < k) {
        auto [lhs, rhs] = split(p->r, k - 1 - (p->l ? p->l->s : 0));
        p->r = lhs;
        if (p->l) p->l->push();
        p->update();
        return {p, rhs};
    } else {
        auto [lhs, rhs] = split(p->l, k);
        p->l = rhs;
        if (p->r) p->r->push();
        p->update();
        return {lhs, p};
    }
}
template <class M>
nodeptr<M> merge(nodeptr<M> lhs, nodeptr<M> rhs) {
    if (lhs) lhs->push();
    if (rhs) rhs->push();
    if (!lhs) return rhs;
    if (!rhs) return lhs;
    if (lhs->p < rhs->p) {
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
template <class M>
nodeptr<M> insert(nodeptr<M> p, int k, typename M::T v) {
    auto [lhs, rhs] = split(p, k);
    return merge(merge(lhs, new node<M>(v)), rhs);
}
template <class M>
nodeptr<M> erase(nodeptr<M> p, int k) {
    auto [lhs, t] = split(p, k);
    auto [mhs, rhs] = split(t, 1);
    delete mhs;
    return merge(lhs, rhs);
}
} // namespace treap_node
// 反転可能treap
// 非可換でも大丈夫
// あくまでtreap_nodeのwrapperみたいな立ち位置
template <class M>
struct treap {
    using T = M::T;
    using node = treap_node::node<M>;
    using nodeptr = node*;
    nodeptr root = nullptr;
    treap() {}
    ~treap() {
        if (root) delete root;
    }
    int size() { return root ? root->s : 0; }
    void insert(int k, T v) { root = treap_node::insert(root, k, v); }
    void push_back(T v) { insert(size(), v); }
    void erase(int k) { root = treap_node::erase(root, k); }
    T prod(int l, int r) {
        if (l == r) return M::e();
        auto [q, rhs] = treap_node::split(root, r);
        auto [lhs, mhs] = treap_node::split(q, l);
        T a = mhs->a.ord;
        root = treap_node::merge(treap_node::merge(lhs, mhs), rhs);
        return a;
    }
    void set(int k, T v) {
        auto [q, rhs] = treap_node::split(root, k + 1);
        auto [lhs, mhs] = treap_node::split(q, k);
        mhs->v = mhs->a = v;
        root = treap_node::merge(treap_node::merge(lhs, mhs), rhs);
    }
    T get(int k) {
        auto [q, rhs] = treap_node::split(root, k + 1);
        auto [lhs, mhs] = treap_node::split(q, k);
        T v = mhs->v.ord;
        root = treap_node::merge(treap_node::merge(lhs, mhs), rhs);
        return v;
    }
    void reverse(int l, int r) {
        if (l == r) return;
        auto [q, rhs] = treap_node::split(root, r);
        auto [lhs, mhs] = treap_node::split(q, l);
        mhs->rev ^= true;
        root = treap_node::merge(treap_node::merge(lhs, mhs), rhs);
    }
    friend ostream& operator<<(ostream& os, treap& t) {
        int n = t.size();
        vector<T> v;
        rep(i, 0, n) { v.push_back(t.get(i)); }
        os << v;
        return os;
    }
};

