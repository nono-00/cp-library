// competitive-verifier: STANDALONE
#include <cassert>
#include <vector>

#include "nono/math/square-free.hpp"

namespace nono {

bool is_square_free_naive(unsigned long long n) {
    for (unsigned long long p = 2; p * p <= n; p++) {
        if (n % (p * p) == 0) return false;
    }
    return true;
}

void solve() {
    //  is_square_free
    for (unsigned long long n = 1; n <= 2000; n++) {
        assert(is_square_free(n) == is_square_free_naive(n));
    }

    //  mobius_table: メビウス関数の定義と突き合わせる
    constexpr unsigned N = 200;
    auto mobius = mobius_table<N>();
    for (unsigned n = 1; n < N; n++) {
        if (!is_square_free_naive(n)) {
            assert(mobius[n] == 0);
            continue;
        }
        unsigned long long x = n;
        int cnt = 0;
        for (unsigned long long p = 2; p * p <= x; p++) {
            while (x % p == 0) {
                x /= p;
                cnt++;
            }
        }
        if (x > 1) cnt++;
        assert(mobius[n] == (cnt % 2 == 0 ? 1 : -1));
    }

    //  count_square_free: [1, 50]の中で愚直に数える
    std::vector<int> naive_count(51, 0);
    for (unsigned long long n = 1; n <= 50; n++) {
        naive_count[n] = naive_count[n - 1] + (is_square_free_naive(n) ? 1 : 0);
    }
    for (unsigned long long n = 1; n <= 50; n++) {
        assert(count_square_free(mobius, n) == (unsigned long long)naive_count[n]);
    }

    //  kth_square_free / kth_non_square_free: 愚直に列挙したk番目と一致するか
    std::vector<unsigned long long> square_frees, non_square_frees;
    for (unsigned long long n = 1; n <= 50; n++) {
        (is_square_free_naive(n) ? square_frees : non_square_frees).push_back(n);
    }
    for (int k = 1; k <= (int)square_frees.size(); k++) {
        assert(kth_square_free(mobius, (unsigned long long)k) == square_frees[k - 1]);
    }
    for (int k = 1; k <= (int)non_square_frees.size(); k++) {
        assert(kth_non_square_free(mobius, (unsigned long long)k) == non_square_frees[k - 1]);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
