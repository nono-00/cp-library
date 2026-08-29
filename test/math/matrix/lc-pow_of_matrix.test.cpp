// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/pow_of_matrix
#include <iostream>

#include "nono/math/matrix.hpp"
#include "nono/math/modint.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    int n;
    long long k;
    std::cin >> n >> k;
    Matrix<Mint> a(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            unsigned x;
            std::cin >> x;
            a[i][j] = x;
        }
    }
    Matrix<Mint> b = a.pow(k);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << b[i][j] << " \n"[j + 1 == n];
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
