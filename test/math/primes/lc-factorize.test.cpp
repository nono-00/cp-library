#define PROBLEM "https://judge.yosupo.jp/problem/factorize"

#include <bits/stdc++.h>

#include "../../../nono/math/primes.hpp"

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    int q;
    std::cin >> q;
    while (q--) {
        unsigned long long x;
        std::cin >> x;
        auto pfs = nono::prime_factors(x);
        std::sort(std::begin(pfs), std::end(pfs));
        std::cout << ssize(pfs);
        for (auto pf: pfs) std::cout << ' ' << pf;
        std::cout << '\n';
    }
}


