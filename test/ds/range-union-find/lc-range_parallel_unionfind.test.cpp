#define PROBLEM "https://judge.yosupo.jp/problem/range_parallel_unionfind"
#include <iostream>
#include <vector>

#include "../../../nono/ds/range-union-find.hpp"
#include "../../../nono/math/modint.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    int n, q;
    std::cin >> n >> q;
    std::vector<Mint> a(n);
    std::vector<Mint> b(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }
    Mint ans = 0;
    RangeUnionFind uf(n);

    auto F = [&](int lhs, int rhs, int par) {
        ans -= b[lhs];
        ans -= b[rhs];
        b[par] = b[lhs] + b[rhs] + a[lhs] * a[rhs];
        a[par] = a[lhs] + a[rhs];
        ans += b[par];
    };

    for (int i = 0; i < q; i++) {
        int k, a, b;
        std::cin >> k >> a >> b;
        uf.merge(a, b, k, F);
        std::cout << ans << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}