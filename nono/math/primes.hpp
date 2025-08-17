#pragma once

#include <array>

namespace nono {

template <int N>
std::array<bool, N> prime_sieve() {
    std::array<bool, N> sieve{};
    sieve.fill(true);
    sieve[0] = sieve[1] = false;
    for (int i = 4; i < N; i += 2) sieve[i] = false;
    for (int i = 3; i * i < N; i += 2) {
        if (sieve[i]) {
            for (int j = i * i; j < N; j += 2 * i) {
                sieve[j] = false;
            }
        }
    }
    return sieve;
}

bool is_prime(unsigned long long p) {
    using u64 = unsigned long long;
    using u128 = __uint128_t;
    if (p < 2) return false;
    if (p == 2) return true;
    if (p % 2 == 0) return false;

    u64 d = p - 1;
    while (d % 2 == 0) d /= 2;

    auto modpow = [&](u64 base, u64 exp, u64 mod) {
        u64 res = 1;
        while (exp) {
            if (exp & 1) {
                res = (u64)((u128)res * base % mod);
            }
            base = (u64)((u128)base * base % mod);
            exp >>= 1;
        }
        return res;
    };

    constexpr std::array<u64, 7> as{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    for (auto a: as) {
        if (a % p == 0) continue;
        u64 e = d;
        u64 v = modpow(a % p, e, p);
        if (v == 1 || v == p - 1) continue;
        e *= 2;
        while (e != p - 1) {
            v = modpow(v, 2, p);
            if (v == p - 1) break;
            e *= 2;
        }
        if (e == p - 1) return false;
    }
    return true;
}

}  //  namespace nono
