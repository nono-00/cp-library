// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_product
#include <iostream>

#include "nono/math/matrix.hpp"
#include "nono/math/modint.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    int n, m, k;
    std::cin >> n >> m >> k;
    Matrix<Mint> a(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            unsigned x;
            std::cin >> x;
            a[i][j] = x;
        }
    }
    Matrix<Mint> b(m, k);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            unsigned x;
            std::cin >> x;
            b[i][j] = x;
        }
    }
    Matrix<Mint> c = a * b;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            std::cout << c[i][j] << " \n"[j + 1 == k];
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
