// competitive-verifier: STANDALONE
#include <cassert>
#include <random>
#include <vector>

#include "nono/math/bitwise-or-convolution.hpp"

namespace nono {

void solve() {
    std::mt19937 mt(12345);
    constexpr int n = 6;
    constexpr int size = 1 << n;
    std::vector<long long> a(size), b(size);
    for (auto& x: a) x = mt() % 10;
    for (auto& x: b) x = mt() % 10;

    //  愚直なOR畳み込み
    std::vector<long long> expected(size, 0);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            expected[i | j] += a[i] * b[j];
        }
    }

    auto c = bitwise_or_convolution(a, b);
    assert(c == expected);
}

}  //  namespace nono

int main() {
    nono::solve();
}
