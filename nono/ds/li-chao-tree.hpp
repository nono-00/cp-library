#pragma once

/// TODO : implementation

#include "../template.hpp"
// https://atcoder.jp/contests/dp/tasks/dp_z
// minだけできる
template <class T>
struct li_chao_tree {
    static constexpr T inf = numeric_limits<T>::max();
    // a x + b
    struct line {
        line(T a = inf, T b = inf): a(a), b(b) {}
        T a, b;
        T eval(ll x) { return a * x + b; }
        friend bool operator==(line lhs, line rhs) { return lhs.a == rhs.a && lhs.b == rhs.b; }
        friend bool operator!=(line lhs, line rhs) { return !(lhs == rhs); }
    };
    int n;
    vector<line> a;
    line none;
    li_chao_tree(int n_): n(bit_ceil((unsigned)n_)), a(2 * n), none() {}
    void add_line(T a, T b) { add_segment(0, n, a, b); }
    // [l, r)にax+bを追加
    void add_segment(int l, int r, T a, T b) { add(0, n, l, r, 1, line(a, b)); }
    T get(int i) { return get(0, n, i, 1); }
   private:
    void add(int l, int r, int L, int R, int i, line v) {
        assert(0 <= l && l < r && r <= n);
        if (r <= L || R <= l) return;
        if (L <= l && r <= R) {
            update(i, l, r, v);
        } else {
            int m = (l + r) / 2;
            add(l, m, L, R, 2 * i, v);
            add(m, r, L, R, 2 * i + 1, v);
        }
    }
    void update(int i, int l, int r, line v) {
        assert(0 <= l && l < r && r <= n);
        if (a[i] == none) {
            a[i] = v;
            return;
        }
        int m = (l + r) / 2;
        bool lhs = a[i].eval(l) < v.eval(l);
        bool mhs = a[i].eval(m) < v.eval(m);
        bool rhs = a[i].eval(r - 1) < v.eval(r - 1);
        if (lhs && mhs && rhs) return;
        if (!lhs && !mhs && !rhs) {
            a[i] = v;
            return;
        }
        if (lhs ^ mhs ^ rhs) {
            swap(v, a[i]);
            lhs ^= true, mhs ^= true, rhs ^= true;
        }
        if (lhs) {
            update(2 * i + 1, m, r, v);
        } else {
            update(2 * i, l, m, v);
        }
    }
    T get(int l, int r, int I, int i) {
        assert(0 <= l && l < r && r <= n);
        ll v = a[i] != none ? a[i].eval(I) : inf;
        if (l == I && l + 1 == r) {
            return v;
        } else {
            int m = (l + r) / 2;
            if (l <= I && I < m) chmin(v, get(l, m, I, 2 * i));
            else chmin(v, get(m, r, I, 2 * i + 1));
            return v;
        }
    }
};
