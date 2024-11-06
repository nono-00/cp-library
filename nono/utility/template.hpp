#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using ld = long double;
using ull = unsigned long long;
template <class T>
using max_heap = priority_queue<T>;
template <class T>
using min_heap = priority_queue<T, vector<T>, greater<T>>;
#define rep(i, l, r) for (ll i = (l); i < (r); i++)
#define rrep(i, r, l) for (ll i = (r); i-- > (l);)
#define all(x) begin(x), end(x)
template <class T>
bool chmin(T& lhs, T rhs) {
    return lhs > rhs ? (lhs = rhs, true) : false;
}
template <class T>
bool chmax(T& lhs, T rhs) {
    return lhs < rhs ? (lhs = rhs, true) : false;
}
struct IOIO {
    IOIO() {
        cin.tie(0)->sync_with_stdio(0);
    }
} ioio;
template <class S, class T>
ostream& operator<<(ostream& os, const pair<S, T>& p) {
    os << '(' << p.first << ", " << p.second << ')';
    return os;
}
template <class T>
ostream& operator<<(ostream& os, const vector<T>& vs) {
    os << '{';
    rep(i, 0, (int)vs.size()) os << vs[i] << (i + 1 == (int)vs.size() ? "" : ", ");
    os << '}';
    return os;
}
template <class T>
ostream& operator<<(ostream& os, const set<T>& vs) {
    os << '{';
    for (auto it = vs.begin(); it != vs.end(); it++) {
        if (it != vs.begin()) {
            os << ", ";
        }
        os << *it;
    }
    os << '}';
    return os;
}
template <class T, class U>
ostream& operator<<(ostream& os, const map<T, U>& vs) {
    os << '{';
    for (auto it = vs.begin(); it != vs.end(); it++) {
        if (it != vs.begin()) {
            os << ", ";
        }
        os << *it;
    }
    os << '}';
    return os;
}
#ifdef DEBUG
void dump_func() {
    cerr << endl;
}
template <class Head, class... Tail>
void dump_func(Head&& head, Tail&&... tail) {
    cerr << head;
    if (sizeof...(Tail) > 0) {
        cerr << ", ";
    }
    dump_func(std::move(tail)...);
}
#define dump(...) cerr << "[" + string(#__VA_ARGS__) + "] ", dump_func(__VA_ARGS__)
#else
#define dump(...) static_cast<int>(0)
#endif
