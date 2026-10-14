#pragma once

#include <array>
#include <cassert>
#include <numeric>
#include <random>
#include <vector>

namespace nono {

//  # THRESHOLD
//  is_primeとprime_factorsが、試し割りと乱択アルゴリズムを切り替える境界
constexpr unsigned THRESHOLD = 100000;

//  # prime_sieve<N>()
//  エラトステネスの篩。result[i] は、iが素数かどうか (0 <= i < N)
//  N >= 2 でないと壊れる
//  O(N log log N)
template <unsigned N>
std::array<bool, N> prime_sieve() {
    std::array<bool, N> sieve{};
    sieve.fill(true);
    sieve[0] = sieve[1] = false;
    for (unsigned i = 4; i < N; i += 2) sieve[i] = false;
    for (unsigned i = 3; i * i < N; i += 2) {
        if (sieve[i]) {
            for (unsigned j = i * i; j < N; j += 2 * i) {
                sieve[j] = false;
            }
        }
    }
    return sieve;
}

//  # min_prime_factor_sieve<N>()
//  result[i] は、iの最小の素因数 (0 <= i < N)。result[0] = 0, result[1] = 1
//  O(N log log N)
template <unsigned N>
std::array<unsigned, N> min_prime_factor_sieve() {
    std::array<unsigned, N> sieve{};
    std::iota(std::begin(sieve), std::end(sieve), 0);
    for (unsigned i = 4; i < N; i += 2) sieve[i] = 2;
    for (unsigned i = 3; i * i < N; i += 2) {
        if (sieve[i] == i) {
            for (unsigned j = i * i; j < N; j += 2 * i) {
                if (sieve[j] == j) sieve[j] = i;
            }
        }
    }
    return sieve;
}

//  # prime_factors<N>(min_pfs, n)
//  min_prime_factor_sieve<N>() の結果を使って、nの素因数を重複込みで昇順に返す
//  Nは引数から推論できないので、明示的に指定する
//  n < N でないと壊れる
//  O(log n)
template <unsigned N>
std::vector<unsigned> prime_factors(const std::array<unsigned, N>& min_pfs, unsigned n) {
    assert(n < N);
    std::vector<unsigned> result;
    while (n > 1) {
        result.push_back(min_pfs[n]);
        n /= min_pfs[n];
    }
    return result;
}

//  # is_prime_probabilistic(p)
//  ミラー・ラビン法による素数判定。底を7個に固定しており、64bitの範囲では誤判定しない
//  O(log p)
bool is_prime_probabilistic(unsigned long long p) {
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

//  # is_prime_deterministic(p)
//  試し割りによる素数判定
//  O(sqrt(p))
bool is_prime_deterministic(unsigned long long p) {
    using u64 = unsigned long long;
    if (p < 2) return false;
    if (p == 2) return true;
    if (p % 2 == 0) return false;
    for (u64 i = 3; i * i <= p; i += 2) {
        if (p % i == 0) return false;
    }
    return true;
}

//  # is_prime(p)
//  pが素数か。p <= THRESHOLD なら試し割り、そうでなければミラー・ラビン法を使う
bool is_prime(unsigned long long p) {
    if (p <= THRESHOLD) {
        return is_prime_deterministic(p);
    } else {
        return is_prime_probabilistic(p);
    }
}

//  # prime_factors_probabilistic(n)
//  ポラードのロー法(乱択)による素因数分解。素因数を重複込みで返す。順序は不定
//  n >= 1 でないと壊れる(n = 0 では終了しない)
std::vector<unsigned long long> prime_factors_probabilistic(unsigned long long n) {
    using u64 = unsigned long long;
    using u128 = __uint128_t;
    std::mt19937_64 mt(std::random_device{}());
    auto pollard = [&mt](u64 n) -> u64 {
        if (is_prime(n)) return n;
        while (true) {
            u64 c = mt() % (n - 1) + 1;
            auto f = [&c, &n](u64 x) {
                return (u64)(((u128)x * x + c) % n);
            };
            u64 x = mt() % n;
            u64 y = x;
            while (true) {
                x = f(x);
                y = f(f(y));
                u64 p = std::gcd(n + y - x, n);
                if (p == n) break;
                if (p != 1) return p;
            }
        }
    };
    std::vector<u64> factors;
    std::vector<u64> result;
    while (n % 2 == 0) result.push_back(2), n /= 2;
    while (n % 3 == 0) result.push_back(3), n /= 3;
    while (n % 5 == 0) result.push_back(5), n /= 5;

    factors.push_back(n);
    while (!std::empty(factors)) {
        auto n = factors.back();
        factors.pop_back();
        if (n == 1) continue;
        auto x = pollard(n);
        if (x == n) {
            result.push_back(n);
            continue;
        }
        factors.push_back(x);
        factors.push_back(n / x);
    }

    return result;
}

//  # prime_factors_deterministic(n)
//  試し割りによる素因数分解。素因数を重複込みで昇順に返す
//  n = 1 なら空、n = 0 なら {0} を返す
//  O(sqrt(n))
std::vector<unsigned long long> prime_factors_deterministic(unsigned long long n) {
    using u64 = unsigned long long;
    std::vector<u64> result;
    for (u64 i = 2; i * i <= n; i++) {
        while (n % i == 0) {
            result.push_back(i);
            n /= i;
        }
    }
    if (n != 1) result.push_back(n);
    return result;
}

//  # prime_factors(n)
//  nの素因数を重複込みで返す。n <= THRESHOLD なら試し割り、そうでなければポラードのロー法を使う
//  順序は不定(試し割りのときだけ昇順になる)
std::vector<unsigned long long> prime_factors(unsigned long long n) {
    if (n <= THRESHOLD) {
        return prime_factors_deterministic(n);
    } else {
        return prime_factors_probabilistic(n);
    }
}

}  //  namespace nono
