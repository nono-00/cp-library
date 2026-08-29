// competitive-verifier: STANDALONE
#include <cassert>
#include <vector>

#include "nono/math/binomial.hpp"

namespace nono {

///  THRESHOLD=1にすることで、小さいnでもbinom_naive経路を強制的に通す
///  issue #20: binom_naiveのkをmin(n-k, k)にする最適化の回帰テスト
void solve() {
    constexpr int MOD = 998244353;
    constexpr int LIM = 300;

    std::vector<std::vector<long long>> pascal(LIM + 1, std::vector<long long>(LIM + 1, 0));
    for (int n = 0; n <= LIM; n++) {
        pascal[n][0] = 1;
        for (int k = 1; k <= n; k++) {
            pascal[n][k] = (pascal[n - 1][k - 1] + (k <= n - 1 ? pascal[n - 1][k] : 0)) % MOD;
        }
    }

    Binomial<MOD, 1> binom;
    for (int n = 0; n <= LIM; n++) {
        for (int k = 0; k <= n; k++) {
            assert(binom.binom(n, k).val() == pascal[n][k]);
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
