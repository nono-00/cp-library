#pragma once
/// TODO : implementation
#include "../template.hpp"
template <class M>
struct segment_tree_beats {
    using T = M::T;
    using F = M::F;
    int n;
    vector<T> a;
    vector<F> lz;
    segment_tree_beats(int n_): n(bit_ceil((unsigned)n_)), a(2 * n, M::e()), lz(n, M::id()) {}
    segment_tree_beats(const vector<T>& a_):
        n(bit_ceil(a_.size())), a(2 * n, M::e()), lz(n, M::id()) {
        copy(all(a_), a.begin() + n);
        rrep(i, n, 1) update(i);
    }
    void apply(int l, int r, F v) { apply(0, n, l, r, 1, v); }
    T prod(int l, int r) { return prod(0, n, l, r, 1); }
    T get(int i) { return prod(0, n, i, i + 1, 1); }
    friend ostream& operator<<(ostream& os, segment_tree_beats& s) {
        rep(i, 0, s.n) s.get(i);
        os << vector(s.a.begin() + s.n, s.a.end());
        return os;
    }
   private:
    void apply(int l, int r, int L, int R, int i, F v) {
        if (r <= L || R <= l) return;
        int m = (l + r) / 2;
        if (L <= l && r <= R) {
            eval(i, v);
        } else {
            push(i);
            apply(l, m, L, R, 2 * i, v);
            apply(m, r, L, R, 2 * i + 1, v);
            update(i);
        }
    }
    T prod(int l, int r, int L, int R, int i) {
        if (r <= L || R <= l) return M::e();
        int m = (l + r) / 2;
        if (L <= l && r <= R) {
            return a[i];
        } else {
            push(i);
            T p = M::op(prod(l, m, L, R, 2 * i), prod(m, r, L, R, 2 * i + 1));
            return p;
        }
    }
    void push(int i) {
        assert(i < n);
        eval(2 * i, lz[i]);
        eval(2 * i + 1, lz[i]);
        lz[i] = M::id();
    }
    void update(int i) {
        assert(i < n);
        a[i] = M::op(a[2 * i], a[2 * i + 1]);
    }
    void eval(int i, F v) {
        a[i] = M::map(v, a[i]);
        if (i < n) {
            lz[i] = M::compose(v, lz[i]);
            if (a[i].fail) {
                push(i);
                update(i);
            }
        }
    }
};

