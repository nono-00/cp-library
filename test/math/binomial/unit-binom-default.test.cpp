// competitive-verifier: STANDALONE
#include <cassert>
#include <vector>

#include "nono/math/binomial.hpp"

namespace nono {

long long modpow(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) res = res * base % mod;
        base = base * base % mod;
        exp >>= 1;
    }
    return res;
}

//  unit-binom-naive.test.cpp（issue #20の回帰テスト、THRESHOLD=1でnaive経路のみ検証）とは別に、
//  実運用で使われるデフォルトTHRESHOLD（extend()による前計算配列）経路のbinom/fact/fact_inv/perm、
//  および法が小さい場合のbinom_lucas経路を検証する
void solve() {
    constexpr long long MOD = 998244353;
    constexpr int LIM = 300;

    std::vector<std::vector<long long>> pascal(LIM + 1, std::vector<long long>(LIM + 1, 0));
    for (int n = 0; n <= LIM; n++) {
        pascal[n][0] = 1;
        for (int k = 1; k <= n; k++) {
            pascal[n][k] = (pascal[n - 1][k - 1] + (k <= n - 1 ? pascal[n - 1][k] : 0)) % MOD;
        }
    }
    std::vector<long long> fact(LIM + 1), fact_inv(LIM + 1);
    fact[0] = 1;
    for (int i = 1; i <= LIM; i++) fact[i] = fact[i - 1] * i % MOD;
    for (int i = 0; i <= LIM; i++) fact_inv[i] = modpow(fact[i], MOD - 2, MOD);

    Binomial<MOD> binom;  //  デフォルトTHRESHOLD(10^7)なのでextend()による前計算経路を通る
    for (int n = 0; n <= LIM; n++) {
        assert(binom.fact(n).val() == (unsigned)fact[n]);
        assert(binom.fact_inv(n).val() == (unsigned)fact_inv[n]);
        for (int k = 0; k <= n; k++) {
            assert(binom.binom(n, k).val() == (unsigned)pascal[n][k]);
            long long perm = fact[n] * fact_inv[n - k] % MOD;
            assert(binom.perm(n, k).val() == (unsigned)perm);
        }
    }

    //  binom_lucas: 法pが小さくn,kの方が大きい場合の経路
    //  Lucasの定理を独立実装したオラクルと突き合わせる
    constexpr int P = 13;
    std::vector<std::vector<long long>> small(P, std::vector<long long>(P, 0));
    for (int n = 0; n < P; n++) {
        small[n][0] = 1;
        for (int k = 1; k <= n; k++) {
            small[n][k] = (small[n - 1][k - 1] + (k <= n - 1 ? small[n - 1][k] : 0)) % P;
        }
    }
    auto lucas = [&](long long n, long long k) -> long long {
        long long res = 1;
        while (n || k) {
            long long ni = n % P, ki = k % P;
            if (ki > ni) return 0;
            res = res * small[ni][ki] % P;
            n /= P;
            k /= P;
        }
        return res;
    };

    Binomial<P> binom_p;
    for (long long n = 0; n <= 1000; n += 7) {
        for (long long k = 0; k <= n; k += 11) {
            assert(binom_p.binom(n, k).val() == (unsigned)lucas(n, k));
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
