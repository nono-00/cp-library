// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>

#include "nono/math/square-sum.hpp"

namespace nono {

bool is_square(unsigned long long x) {
    unsigned long long r = (unsigned long long)std::sqrt((double)x);
    for (unsigned long long c = (r > 1 ? r - 1 : 0); c <= r + 1; c++) {
        if (c * c == x) return true;
    }
    return false;
}

//  min k s.t. n = a1^2 + ... + ak^2 (愚直、k<=3の範囲だけ探索し、無ければ4)
int count_square_sum_terms_naive(unsigned long long n) {
    if (is_square(n)) return 1;
    for (unsigned long long a = 0; a * a <= n; a++) {
        if (is_square(n - a * a)) return 2;
    }
    for (unsigned long long a = 0; a * a <= n; a++) {
        for (unsigned long long b = a; a * a + b * b <= n; b++) {
            if (is_square(n - a * a - b * b)) return 3;
        }
    }
    return 4;
}

void solve() {
    for (unsigned long long n = 1; n <= 300; n++) {
        assert(count_square_sum_terms(n) == count_square_sum_terms_naive(n));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
