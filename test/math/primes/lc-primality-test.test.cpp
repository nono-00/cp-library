#define PROBLEM "https://judge.yosupo.jp/problem/primality_test"

#include <bits/stdc++.h>

#include "nono/math/primes.hpp"

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    int q;
    std::cin >> q;
    while (q--) {
        unsigned long long n;
        std::cin >> n;
        std::cout << (nono::is_prime(n) ? "Yes" : "No") << '\n';
    }
}
