#define PROBLEM "https://judge.yosupo.jp/problem/bitwise_and_convolution"
#include <iostream>
#include <vector>

#include "nono/math/bitwise-and-convolution.hpp"
#include "nono/math/modint.hpp"

namespace nono {

void solve() {
    using Mint = nono::Modint998244353;
    int n;
    std::cin >> n;
    std::vector<Mint> A(1 << n), B(1 << n);
    for (int i = 0; i < (1 << n); i++) std::cin >> A[i];
    for (int i = 0; i < (1 << n); i++) std::cin >> B[i];
    auto C = nono::bitwise_and_convolution(A, B);
    for (int i = 0; i < (1 << n); i++) std::cout << C[i] << (i + 1 == (1 << n) ? '\n' : ' ');
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
