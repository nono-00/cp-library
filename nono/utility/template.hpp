#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using ld = long double;
using ull = unsigned long long;
template <class T>
using MaxHeap = std::priority_queue<T>;
template <class T>
using MinHeap = std::priority_queue<T, vector<T>, greater<T>>;
#define rep2(i, n) for (ll i = 0; i < (n); i++)
#define rep3(i, l, r) for (ll i = (l); i < (r); i++)
#define rrep2(i, n) for (ll i = n; i-- > 0;)
#define rrep3(i, r, l) for (ll i = (r); i-- > (l);)
#define overload(a, b, c, d, ...) d
#define rep(...) overload(__VA_ARGS__, rep3, rep2)(__VA_ARGS__)
#define rrep(...) overload(__VA_ARGS__, rrep3, rrep2)(__VA_ARGS__)
#define all(x) begin(x), end(x)
bool chmin(auto& lhs, auto rhs) {
    return lhs > rhs ? lhs = rhs, 1 : 0;
}
bool chmax(auto& lhs, auto rhs) {
    return lhs < rhs ? lhs = rhs, 1 : 0;
}
struct IOIO {
    IOIO() {
        std::cin.tie(0)->sync_with_stdio(0);
    }
} ioio;
