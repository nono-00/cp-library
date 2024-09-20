#define PROBLEM "https://judge.yosupo.jp/problem/staticrmq"

#include <iostream>
#include <limits>
#include <vector>

#include "nono/ds/sparse-table.hpp"

namespace nono {

template <class T>
struct Min {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::min(lhs, rhs);
    }
    static Value e() {
        return std::numeric_limits<T>::max();
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    SparseTable<Min<int>> sparse(a);
    for (int i = 0; i < q; i++) {
        int l, r;
        std::cin >> l >> r;
        std::cout << sparse.prod(l, r) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
