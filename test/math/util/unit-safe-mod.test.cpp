// competitive-verifier: STANDALONE
#include <cassert>

#include "nono/math/util.hpp"

namespace nono {

void solve() {
    //  safe_mod(x, m): 結果は常に[0, m)（C++の%と異なりxが負でも非負を返す）
    for (long long m = 1; m <= 20; m++) {
        for (long long x = -100; x <= 100; x++) {
            long long r = safe_mod(x, m);
            assert(0 <= r && r < m);
            //  x - r は m の倍数
            assert((x - r) % m == 0);
        }
    }

    assert(safe_mod(7, 3) == 1);
    assert(safe_mod(-7, 3) == 2);
    assert(safe_mod(-1, 5) == 4);
    assert(safe_mod(0, 5) == 0);
}

}  //  namespace nono

int main() {
    nono::solve();
}
