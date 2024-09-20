#define PROBLEM "https://judge.yosupo.jp/problem/static_range_sum"
#include <iostream>
#include <vector>

#include "nono/ds/disjoint-sparse-table.hpp"

namespace nono {

template <class T>
struct Add {
    using Value = T;
    static T op(T lhs, T rhs) {
        return lhs + rhs;
    }
    static T e() {
        return T(0);
    }
    static T inv(T elem) {
        return -elem;
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    DisjointSparseTable<Add<long long>> table(a);
    while (q--) {
        int l, r;
        std::cin >> l >> r;
        std::cout << table.prod(l, r) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
