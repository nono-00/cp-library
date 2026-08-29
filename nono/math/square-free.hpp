#pragma once

#include <algorithm>
#include <array>
#include <cassert>

#include "primes.hpp"

namespace nono {

bool is_square_free(unsigned long long n) {
    auto factors = prime_factors(n);
    std::sort(std::begin(factors), std::end(factors));
    for (int i = 0; i + 1 < ssize(factors); i++) {
        if (factors[i] == factors[i + 1]) return false;
    }
    return true;
}

//  O(N log log N)
//  table[i]:
//      i is non square free = 0
//      has odd num of primes = -1
//      has even num of primes = 1
template <unsigned N>
std::array<int, N> mobius_table() {
    using u32 = unsigned;
    using u64 = unsigned long long;
    std::array<int, N> table;
    std::array<bool, N> sieve;
    table.fill(1);
    sieve.fill(true);
    for (u32 i = 2; i < N; i++) {
        if (sieve[i]) {
            for (u32 j = i; j < N; j += i) {
                table[j] *= -1;
                sieve[j] = false;
            }
            u64 square = (u64)i * i;
            for (u64 j = square; j < N; j += square) {
                table[j] = 0;
            }
        }
    }
    return table;
}

//  #[ x | x <= n, x is square free]
//  O(sqrt(n))
unsigned long long count_square_free(const auto& mobius, unsigned long long n) {
    assert(n > 0);
    const unsigned N = std::ssize(mobius);
    long long ans = 0;
    for (unsigned long long i = 1; i * i <= n; i++) {
        assert(i < N);
        if (mobius[i]) ans += mobius[i] * (long long)(n / (i * i));
    }
    return ans;
}

//  square free : 2, 3, 5, 6, 7, 10, 11, ...
//  k : 1-origin
//  ex.
//  kth_square_free(_, 1) := 2
unsigned long long kth_square_free(const auto& mobius, unsigned long long k) {
    assert(k > 0);
    using u64 = unsigned long long;
    const unsigned N = std::ssize(mobius);
    u64 ok = (u64)N * N;
    u64 ng = 0;
    while (ok - ng > 1) {
        u64 key = std::midpoint(ok, ng);
        if (count_square_free(mobius, key) >= k) {
            ok = key;
        } else {
            ng = key;
        }
    }
    return ok;
}

//  k : 1-origin
//  ex.
//  kth_non_square_free(_, 1) := 1
unsigned long long kth_non_square_free(const auto& mobius, unsigned long long k) {
    assert(k > 0);
    using u64 = unsigned long long;
    const unsigned N = std::ssize(mobius);
    u64 ok = (u64)N * N;
    u64 ng = 0;
    while (ok - ng > 1) {
        u64 key = std::midpoint(ok, ng);
        if (key - count_square_free(mobius, key) >= k) {
            ok = key;
        } else {
            ng = key;
        }
    }
    return ok;
}

}  //  namespace nono
