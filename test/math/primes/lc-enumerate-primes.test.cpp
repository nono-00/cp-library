#define PROBLEM "https://judge.yosupo.jp/problem/enumerate_primes"

#include <bits/stdc++.h>

#include "../../../nono/math/primes.hpp"

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    constexpr int N = 500000010;
    static auto sieve = nono::prime_sieve<N>();
    int n, a, b;
    std::cin >> n >> a >> b;
    std::vector<int> primes;
    for (int i = 2, j = 0; i <= n; i++) {
        if (!sieve[i]) continue;
        if (j == b) primes.push_back(i);
        j++;
        if (j == a) j = 0;
    }
    std::cout << std::count(std::begin(sieve), std::begin(sieve) + n + 1, true) << ' ' << ssize(primes) << '\n';
    for (int i = 0; i < ssize(primes); i++) std::cout << primes[i] << " \n"[i + 1 == ssize(primes)];
}

