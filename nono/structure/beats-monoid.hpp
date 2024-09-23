#pragma once
#include "./../template.hpp"
namespace beats_monoid {
template <class U>
struct chmin_sum {
    struct info {
        using T = U;
        info(T a = numeric_limits<T>::min()) {
            v[0] = a;
            v[1] = numeric_limits<T>::min();
            n = 1;
            s = a;
            fail = false;
        }
        // v[0]: maximum value, v[1]: second maximum value
        array<U, 2> v;
        // n: num of maximum value
        int n;
        // s: sum of segment
        U s;
        // success or fail to apply
        bool fail;
        friend ostream& operator<<(ostream& os, info i) {
            os << i.s;
            return os;
        }
    };
    using T = info;
    using F = T;
    static T op(T lhs, T rhs) {
        T a;
        int l = 0, r = 0;
        a.s = lhs.s + rhs.s;
        a.fail = false;
        rep(i, 0, 2) {
            if (lhs.v[l] == rhs.v[r]) {
                a.v[i] = lhs.v[l];
                if (i == 0) a.n = lhs.n + rhs.n;
                l++, r++;
            } else if (lhs.v[l] < rhs.v[r]) {
                a.v[i] = rhs.v[r];
                if (i == 0) a.n = rhs.n;
                r++;
            } else {
                a.v[i] = lhs.v[l];
                if (i == 0) a.n = lhs.n;
                l++;
            }
        }
        return a;
    }
    static T e() {
        T a;
        a.v[0] = a.v[1] = numeric_limits<T>::min();
        a.n = 0;
        a.s = 0;
        a.fail = false;
        return a;
    }
    static T map(F f, T v) {
        if (f >= v.v[0]) {
            v.fail = false;
            return v;
        } else if (f > v.v[1]) {
            v.s -= (v.v[0] - f) * v.n;
            v.v[0] = f;
            return v;
        } else {
            v.fail = true;
            return v;
        }
    }
    static F compose(F lhs, F rhs) { return min(lhs, rhs); }
    static F id() { return numeric_limits<F>::max(); }
};
template <class U>
struct chmax_sum {
    struct info {
        using T = U;
        info(T a = numeric_limits<T>::max()) {
            v[0] = a;
            v[1] = numeric_limits<T>::max();
            n = 1;
            s = a;
            fail = false;
        }
        // v[0]: minimum value, v[1]: second minimum value
        array<U, 2> v;
        // n: num of minimum value
        int n;
        // s: sum of segment
        U s;
        // success or fail to apply
        bool fail;
        friend ostream& operator<<(ostream& os, info i) {
            os << i.s;
            return os;
        }
    };
    using T = info;
    using F = U;
    static T op(T lhs, T rhs) {
        T a;
        int l = 0, r = 0;
        a.s = lhs.s + rhs.s;
        a.fail = false;
        rep(i, 0, 2) {
            if (lhs.v[l] == rhs.v[r]) {
                a.v[i] = lhs.v[l];
                if (i == 0) a.n = lhs.n + rhs.n;
                l++, r++;
            } else if (lhs.v[l] > rhs.v[r]) {
                a.v[i] = rhs.v[r];
                if (i == 0) a.n = rhs.n;
                r++;
            } else {
                a.v[i] = lhs.v[l];
                if (i == 0) a.n = lhs.n;
                l++;
            }
        }
        return a;
    }
    static T e() {
        T a;
        a.v[0] = a.v[1] = numeric_limits<F>::max();
        a.n = 0;
        a.s = 0;
        a.fail = false;
        return a;
    }
    static T map(F f, T v) {
        if (f <= v.v[0]) {
            v.fail = false;
            return v;
        } else if (f < v.v[1]) {
            v.s += (f - v.v[0]) * v.n;
            v.v[0] = f;
            return v;
        } else {
            v.fail = true;
            return v;
        }
    }
    static F compose(F lhs, F rhs) { return max(lhs, rhs); }
    static F id() { return numeric_limits<F>::min(); }
};
template <class U>
struct chmin_chmax_sum {
    struct ope {
        using T = U;
        // a: chmin
        // b: chmax
        T a, b;
        ope(T a, T b): a(a), b(b) {}
        static ope chmin(T v) { return ope(v, numeric_limits<T>::min()); }
        static ope chmax(T v) { return ope(numeric_limits<T>::max(), v); }
    };
    struct info {
        using T = U;
        info(T v = 0) {
            a[0] = b[0] = v;
            a[1] = numeric_limits<T>::min();
            b[1] = numeric_limits<T>::max();
            n = 1;
            m = 1;
            s = v;
            fail = false;
        }
        // a[0]: maximum value, v[1]: second maximum value
        array<T, 2> a;
        // b[0]: minimum value, v[1]: second minimum value
        array<T, 2> b;
        // n: num of maximum value
        int n;
        // m: num of minimum value
        int m;
        // s: sum of segment
        T s;
        // success or fail to apply
        bool fail;
        friend ostream& operator<<(ostream& os, info i) {
            os << i.s;
            return os;
        }
    };
    using T = info;
    using F = ope;
    static T op(T lhs, T rhs) {
        T a;
        a.s = lhs.s + rhs.s;
        a.fail = false;
        { // min
            int l = 0, r = 0;
            rep(i, 0, 2) {
                if (lhs.a[l] == rhs.a[r]) {
                    a.a[i] = lhs.a[l];
                    if (i == 0) a.n = lhs.n + rhs.n;
                    l++, r++;
                } else if (lhs.a[l] < rhs.a[r]) {
                    a.a[i] = rhs.a[r];
                    if (i == 0) a.n = rhs.n;
                    r++;
                } else {
                    a.a[i] = lhs.a[l];
                    if (i == 0) a.n = lhs.n;
                    l++;
                }
            }
        }
        { // max
            int l = 0, r = 0;
            rep(i, 0, 2) {
                if (lhs.b[l] == rhs.b[r]) {
                    a.b[i] = lhs.b[l];
                    if (i == 0) a.m = lhs.m + rhs.m;
                    l++, r++;
                } else if (lhs.b[l] > rhs.b[r]) {
                    a.b[i] = rhs.b[r];
                    if (i == 0) a.m = rhs.m;
                    r++;
                } else {
                    a.b[i] = lhs.b[l];
                    if (i == 0) a.m = lhs.m;
                    l++;
                }
            }
        }
        return a;
    }
    static T e() {
        T a;
        a.a[0] = a.a[1] = numeric_limits<U>::min();
        a.b[0] = a.b[1] = numeric_limits<U>::max();
        a.n = 0;
        a.m = 0;
        a.s = 0;
        a.fail = false;
        return a;
    }
    static T map(F f, T a) {
        if (f.a <= a.a[1] || f.b >= a.b[1]) {
            a.fail = true;
            return a;
        }
        if (a.a[0] > f.a && f.a > a.a[1]) {
            a.s -= (a.a[0] - f.a) * a.n;
            if (a.a[0] == a.b[1]) {
                a.b[1] = f.a;
            } else if (a.a[0] == a.b[0]) {
                a.b[0] = f.a;
            }
            a.a[0] = f.a;
        }
        if (f.b >= a.b[1]) {
            a.fail = true;
            return a;
        }
        if (a.b[0] < f.b && f.b < a.b[1]) {
            a.s += (f.b - a.b[0]) * a.m;
            if (a.b[0] == a.a[1]) {
                a.a[1] = f.b;
            } else if (a.a[0] == a.b[0]) {
                a.a[0] = f.b;
            }
            a.b[0] = f.b;
        }
        return a;
    }
    static F compose(F lhs, F rhs) {
        if (lhs.a <= rhs.b) return F(lhs.a, lhs.a);
        if (rhs.a <= lhs.b) return F(lhs.b, lhs.b);
        return F(min(lhs.a, rhs.a), max(lhs.b, rhs.b));
    }
    static F id() { return F(numeric_limits<U>::max(), numeric_limits<U>::min()); }
};
template <class U>
struct chmin_chmax_add_sum {
    struct ope {
        using T = U;
        // a: chmin
        // b: chmax
        // c: add
        T a, b, c;
        ope() {}
        ope(T a, T b, T c): a(a), b(b), c(c) {}
        static ope chmin(T v) { return ope(v, numeric_limits<T>::min(), 0); }
        static ope chmax(T v) { return ope(numeric_limits<T>::max(), v, 0); }
        static ope add(T v) { return ope(numeric_limits<T>::max(), numeric_limits<T>::min(), v); }
    };
    struct info {
        using T = U;
        info(T v = 0) {
            a[0] = b[0] = v;
            a[1] = numeric_limits<T>::min();
            b[1] = numeric_limits<T>::max();
            n = 1;
            m = 1;
            l = 1;
            s = v;
            fail = false;
        }
        // a[0]: maximum value, v[1]: second maximum value
        array<T, 2> a;
        // b[0]: minimum value, v[1]: second minimum value
        array<T, 2> b;
        // n: num of maximum value
        int n;
        // m: num of minimum value
        int m;
        // l: num of value
        int l;
        // s: sum of segment
        T s;
        // success or fail to apply
        bool fail;
        friend ostream& operator<<(ostream& os, info i) {
            os << i.s;
            return os;
        }
    };
    using T = info;
    using F = ope;
    static T op(T lhs, T rhs) {
        T a;
        a.s = lhs.s + rhs.s;
        a.l = lhs.l + rhs.l;
        a.fail = false;
        { // min
            int l = 0, r = 0;
            rep(i, 0, 2) {
                if (lhs.a[l] == rhs.a[r]) {
                    a.a[i] = lhs.a[l];
                    if (i == 0) a.n = lhs.n + rhs.n;
                    l++, r++;
                } else if (lhs.a[l] < rhs.a[r]) {
                    a.a[i] = rhs.a[r];
                    if (i == 0) a.n = rhs.n;
                    r++;
                } else {
                    a.a[i] = lhs.a[l];
                    if (i == 0) a.n = lhs.n;
                    l++;
                }
            }
        }
        { // max
            int l = 0, r = 0;
            rep(i, 0, 2) {
                if (lhs.b[l] == rhs.b[r]) {
                    a.b[i] = lhs.b[l];
                    if (i == 0) a.m = lhs.m + rhs.m;
                    l++, r++;
                } else if (lhs.b[l] > rhs.b[r]) {
                    a.b[i] = rhs.b[r];
                    if (i == 0) a.m = rhs.m;
                    r++;
                } else {
                    a.b[i] = lhs.b[l];
                    if (i == 0) a.m = lhs.m;
                    l++;
                }
            }
        }
        return a;
    }
    static T e() {
        T a;
        a.a[0] = a.a[1] = numeric_limits<U>::min();
        a.b[0] = a.b[1] = numeric_limits<U>::max();
        a.n = 0;
        a.m = 0;
        a.s = 0;
        a.l = 0;
        a.fail = false;
        return a;
    }
    static T map(F f, T a) {
        if (f.a <= a.a[1]) {
            a.fail = true;
            return a;
        }
        if (a.a[0] > f.a && f.a > a.a[1]) {
            a.s -= (a.a[0] - f.a) * a.n;
            if (a.a[0] == a.b[1]) {
                a.b[1] = f.a;
            } else if (a.a[0] == a.b[0]) {
                a.b[0] = f.a;
            }
            a.a[0] = f.a;
        }
        if (f.b >= a.b[1]) {
            a.fail = true;
            return a;
        }
        if (a.b[0] < f.b && f.b < a.b[1]) {
            a.s += (f.b - a.b[0]) * a.m;
            if (a.b[0] == a.a[1]) {
                a.a[1] = f.b;
            } else if (a.a[0] == a.b[0]) {
                a.a[0] = f.b;
            }
            a.b[0] = f.b;
        }
        a.s += f.c * a.l;
        rep(i, 0, 2) {
            if (a.a[i] != numeric_limits<U>::min()) a.a[i] += f.c;
            if (a.b[i] != numeric_limits<U>::max()) a.b[i] += f.c;
        }
        return a;
    }
    static F compose(F lhs, F rhs) {
        F mhs;
        mhs.a = (lhs.a != numeric_limits<U>::max() ? lhs.a - rhs.c : lhs.a);
        mhs.b = (lhs.b != numeric_limits<U>::min() ? lhs.b - rhs.c : lhs.b);
        if (rhs.a <= mhs.b) return F(mhs.b, mhs.b, lhs.c + rhs.c);
        if (mhs.a <= rhs.b) return F(mhs.a, mhs.a, lhs.c + rhs.c);
        return F(min(rhs.a, mhs.a), max(rhs.b, mhs.b), lhs.c + rhs.c);
    }
    static F id() { return F(numeric_limits<U>::max(), numeric_limits<U>::min(), 0); }
};
// https://atcoder.jp/contests/abc256/tasks/abc256_h
struct divide_update_sum {
    static constexpr ll divide_id = 1;
    static constexpr ll update_id = -1;
    struct ope {
        // a: divide
        // b: update
        ll a, b;
        ope(ll a, ll b): a(a), b(b) {}
        static ope divide(ll a) { return ope(a, update_id); };
        static ope update(ll b) { return ope(divide_id, b); };
        friend bool operator==(ope lhs, ope rhs) { return lhs.a == rhs.a && lhs.b == rhs.b; }
        friend bool operator!=(ope lhs, ope rhs) { return !(lhs == rhs); }
    };
    struct info {
        info(ll v = 0): a(v), s(v), n(1), same(true), fail(false) {}
        // a: most left value
        ll a;
        // s: sum of segment
        ll s;
        // n: length of segment
        ll n;
        // same: all i in [l, r) a_i == a
        bool same;
        bool fail;
        friend ostream& operator<<(ostream& os, const info& info) {
            os << info.s;
            return os;
        }
    };
    using T = info;
    using F = ope;
    static T op(T lhs, T rhs) {
        T r;
        r.a = lhs.a;
        r.s = lhs.s + rhs.s;
        r.n = lhs.n + rhs.n;
        r.same = lhs.same && rhs.same && lhs.a == rhs.a;
        r.fail = false;
        return r;
    }
    static T e() {
        T a;
        a.a = 0;
        a.s = 0;
        a.n = 0;
        a.same = true;
        a.fail = false;
        return a;
    }
    static T map(F f, T a) {
        if (f == id()) return a;
        if (!a.same) {
            a.fail = true;
            return a;
        }
        if (f.b != update_id) { a.a = f.b; }
        a.a /= f.a;
        a.s = a.a * a.n;
        return a;
    }
    static F compose(F lhs, F rhs) {
        if (lhs.b != update_id) return lhs;
        return {min(lhs.a * rhs.a, (ll)1e6), rhs.b};
    }
    static F id() { return ope(divide_id, update_id); }
};
// https://codeforces.com/contest/855/problem/F
template <class U>
struct chmin_boot_sum {
    struct ope {
        using T = U;
        T a;
        bool b;
        ope(T a, bool b): a(a), b(b) {}
        static ope chmin(T a) { return ope(a, false); }
        static ope boot() { return ope(numeric_limits<T>::max(), true); }
    };
    struct info {
        using T = U;
        info(T a = numeric_limits<T>::min()) {
            v[0] = a;
            v[1] = numeric_limits<T>::min();
            n = 1;
            s = 0;
            fail = false;
        }
        // v[0]: maximum value, v[1]: second maximum value
        array<U, 2> v;
        // n: num of maximum value
        int n;
        // s: sum of segment
        U s;
        // success or fail to apply
        bool fail;
        friend ostream& operator<<(ostream& os, info i) {
            os << '(' << i.s << ' ' << i.n << ' ' << i.v[0] << ' ' << i.v[1] << ')' << ' ';
            return os;
        }
    };
    using T = info;
    using F = ope;
    static T op(T lhs, T rhs) {
        T a;
        int l = 0, r = 0;
        a.s = lhs.s + rhs.s;
        a.fail = false;
        rep(i, 0, 2) {
            if (lhs.v[l] == rhs.v[r]) {
                a.v[i] = lhs.v[l];
                if (i == 0) a.n = lhs.n + rhs.n;
                l++, r++;
            } else if (lhs.v[l] < rhs.v[r]) {
                a.v[i] = rhs.v[r];
                if (i == 0) a.n = rhs.n;
                r++;
            } else {
                a.v[i] = lhs.v[l];
                if (i == 0) a.n = lhs.n;
                l++;
            }
        }
        return a;
    }
    static T e() {
        T a;
        a.v[0] = numeric_limits<U>::max();
        a.v[1] = numeric_limits<U>::min();
        a.n = 0;
        a.s = 0;
        a.fail = false;
        return a;
    }
    static T map(F f, T v) {
        if (f.a >= v.v[0]) {
            if (f.b) {
                v.n = 1;
                v.s = v.v[0];
            }
            return v;
        } else if (f.a > v.v[1]) {
            v.s -= (v.v[0] - f.a) * v.n;
            v.v[0] = f.a;
            if (f.b) {
                v.n = 1;
                v.s = v.v[0];
            }
            return v;
        } else {
            assert(!f.b);
            v.fail = true;
            return v;
        }
    }
    static F compose(F lhs, F rhs) { return F(min(lhs.a, rhs.a), lhs.b || rhs.b); }
    static F id() { return F(numeric_limits<U>::max(), false); }
};
} // namespace beats_monoid
