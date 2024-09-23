#define PROBLEM "https://judge.yosupo.jp/problem/staticrmq"

#include <iostream>
#include <vector>

#include "nono/ds/disjoint-sparse-table.hpp"
#include "nono/structure/monoid.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    using Monoid = monoid::Min<int>;
    using Value = Monoid::Value;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    DisjointSparseTable<Monoid> sparse(std::vector<Value>(a.begin(), a.end()));
    for (int i = 0; i < q; i++) {
        int l, r;
        std::cin >> l >> r;
        std::cout << sparse.prod(l, r).value() << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
