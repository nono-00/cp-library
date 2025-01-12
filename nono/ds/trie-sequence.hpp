#pragma once

#include <limits>

// #include "../structure/monoid.hpp"

namespace nono {

namespace sortable_sequence_node {

namespace binary_trie_node {

template <class M, class Key = unsigned, int W = std::numeric_limits<Key>::digits>
struct Node {
    using Rev = monoid::Rev<M>;
    Node() = default;
    Node(M::Value value): total(value) {}

    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;
    Rev::Value total;
};

template <class M, class Key = unsigned, int W = std::numeric_limits<Key>::digits>
using NodePtr = Node<M, Key, W>*;

template <class M, class Key = unsigned, int W = std::numeric_limits<Key>::digits>
int size(NodePtr<M, Key, W> root) {
    return root ? root->size : 0;
}

template <class M, class Key = unsigned, int W = std::numeric_limits<Key>::digits>
monoid::Rev<M>::Value prod(NodePtr<M, Key, W> root) {
    return root ? root->total : M::e();
}

template <class M, class Key = unsigned, int W = std::numeric_limits<Key>::digits>
void update(NodePtr<M, Key, W> root) {
    root->size = size(root->left) + size(root->right);
    root->total = monoid::Rev<M>::op(prod(root->left), prod(root->right));
}


}  //  namespace binary_trie_node

}  //  namespace sortable_sequence_node

}  //  namespace nono

///  TODO : implementation
//  #include "../monoid.hpp"
//  #include "../template.hpp"
//  #include "./segment_tree.hpp"
//  https://judge.yosupo.jp/problem/point_set_range_sort_range_composite
//  trie列を管理する
//  ソートのキーとなる値はdistinct
//  一点更新 区間ソート 区間積
template <class M, class K = unsigned, int lg = 31>
struct trie_sequence {
    struct node {
        using nodeptr = node*;
        using MM = monoid::with_rev<M>;
        using TT = MM::T;
        TT v;
        int s = 0;
        nodeptr l = nullptr, r = nullptr;
        node(): v(MM::e()) {}
        node(TT v): v(v) {}
        ~node() {
            if (l) delete l;
            if (r) delete r;
        }
        void update() {
            v = MM::e();
            s = 0;
            if (l) v = MM::op(v, l->v), s += l->s;
            if (r) v = MM::op(v, r->v), s += r->s;
        }
        static pair<nodeptr, nodeptr> split(nodeptr p, int k) {
            assert(p->s >= k);
            if (p->s == k) return {p, nullptr};
            if (k == 0) return {nullptr, p};
            nodeptr q = new node();
            if (!p->l || p->l->s < k) {
                auto [lhs, rhs] = split(p->r, k - (p->l ? p->l->s : 0));
                p->r = lhs;
                q->r = rhs;
            } else {
                auto [lhs, rhs] = split(p->l, k);
                p->l = lhs;
                q->l = rhs;
                swap(p->r, q->r);
            }
            p->update();
            q->update();
            assert(p->s == k);
            return {p, q};
        }
        static nodeptr meld(nodeptr x, nodeptr y) {
            if (!y) return x;
            if (!x) return y;
            x->l = meld(x->l, y->l);
            x->r = meld(x->r, y->r);
            x->update();
            y->l = y->r = nullptr;
            delete y;
            return x;
        }
    };
    using nodeptr = node*;
    using T = M::T;
    //  n: 列の長さ
    int n;
    //  s: 各区間の左端を管理する
    std::set<int> s;
    //  root: 各区間の左端はtrieの根
    vector<nodeptr> root;
    //  rev: 各trieが昇順か降順か
    vector<bool> rev;
    //  segtree: trieの根の総積を管理するセグ木
    segment_tree<M> segtree;
    trie_sequence(const vector<K>& k, const vector<T>& v): n(k.size()), root(n), rev(n), segtree(n) {
        rep(i, 0, n + 1) s.insert(i);
        rep(i, 0, n) {
            root[i] = chain(k[i], v[i]);
            rev[i] = false;
            segtree.set(i, v[i]);
        }
    }
    ~trie_sequence() noexcept {
        rep(i, 0, n) if (root[i]) {
            delete root[i];
        }
    }
    //  (key = x, value = v)のみ持つtrieを作成
    static nodeptr chain(unsigned x, T v) {
        return chain(x, v, lg);
    }
    static nodeptr chain(unsigned x, T v, int d) {
        nodeptr p = new node();
        if (d == 0) {
            p->s = 1;
            p->v = v;
            return p;
        }
        d--;
        ((x >> d) & 1 ? p->r : p->l) = chain(x, v, d);
        p->update();
        return p;
    }
    //  |0|1|2|3|4|
    //  iの含まれる区間[l, r)を[l, i), [i, r)で分割する
    //  すでに分割されている場合、なにもしない
    void split(int i) {
        if (i == 0 || i == n) return;
        auto it = s.upper_bound(i);
        int r = *it;
        int l = *prev(it);
        assert(l <= i);
        if (l == i) return;
        s.insert(i);
        if (rev[l]) {
            auto [lhs, rhs] = node::split(root[l], r - i);
            root[l] = rhs;
            root[i] = lhs;
        } else {
            auto [lhs, rhs] = node::split(root[l], i - l);
            root[l] = lhs;
            root[i] = rhs;
        }
        rev[i] = rev[l];
        if (rev[l]) {
            segtree.set(l, root[l]->v.rev);
            segtree.set(i, root[i]->v.rev);
        } else {
            segtree.set(l, root[l]->v.ord);
            segtree.set(i, root[i]->v.ord);
        }
    }
    //  [l, r)をソートする <=> 区間[l, r)に対応するノードを作成する
    //  ソートのキーとなる値はdistinct
    //  f = true -> 降順
    void sort(int l, int r, bool f = false) {
        split(l);
        split(r);
        for (auto it = s.upper_bound(l); *it < r; it = s.erase(it)) {
            root[l] = node::meld(root[l], root[*it]);
            root[*it] = nullptr;
            segtree.set(*it, M::e());
        }
        rev[l] = f;
        if (f) {
            segtree.set(l, root[l]->v.rev);
        } else {
            segtree.set(l, root[l]->v.ord);
        }
    }
    //  iの要素を変更する
    void set(int i, unsigned k, T v) {
        split(i);
        split(i + 1);
        assert(root[i]);
        delete root[i];
        root[i] = chain(k, v);
        rev[i] = false;
        segtree.set(i, v);
    }
    //  [l, r)の総積
    T prod(int l, int r) {
        split(l);
        split(r);
        return segtree.prod(l, r);
    }
    T get(int i) {
        return prod(i, i + 1);
    }
    friend ostream& operator<<(ostream& os, trie_sequence& s) {
        vector<T> a;
        rep(i, 0, s.n) a.push_back(s.get(i));
        os << a;
        return os;
    }
};
