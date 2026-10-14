#pragma once

#include <algorithm>
#include <array>
#include <cassert>

#include "./primes.hpp"

namespace nono {

//  # is_square_free(n)
//  nが平方因子を持たない(1より大きい平方数で割り切れない)か
//  計算量は prime_factors(n) と同じ
bool is_square_free(unsigned long long n) {
    auto factors = prime_factors(n);
    std::sort(std::begin(factors), std::end(factors));
    for (int i = 0; i + 1 < ssize(factors); i++) {
        if (factors[i] == factors[i + 1]) return false;
    }
    return true;
}

//  # mobius_table<N>()
//  メビウス関数の表。table[i] (1 <= i < N) は、
//  iが平方因子を持つなら0、相異なる素因数が奇数個なら-1、偶数個なら1
//  O(N log log N)
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

//  # count_square_free(mobius, n)
//  1以上n以下の、平方因子を持たない整数の個数を返す。mobiusは mobius_table の結果
//  n > 0 でないと壊れる。sqrt(n) がmobiusの長さ以上だと壊れる
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

//  # kth_square_free(mobius, k)
//  平方因子を持たない整数 1, 2, 3, 5, 6, 7, 10, 11, ... のうち、k番目(1始まり)を返す
//  ex.
//  kth_square_free(_, 1) := 1
//  k > 0 でないと壊れる。答えが N^2 (N: mobiusの長さ) を超えると壊れる
//  O(N log N)
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

//  # kth_non_square_free(mobius, k)
//  平方因子を持つ整数 4, 8, 9, 12, 16, 18, ... のうち、k番目(1始まり)を返す
//  ex.
//  kth_non_square_free(_, 1) := 4
//  k > 0 でないと壊れる。答えが N^2 (N: mobiusの長さ) を超えると壊れる
//  O(N log N)
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
