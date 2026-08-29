// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>

#include "nono/algo/binary-search.hpp"

namespace nono {

void solve() {
    //  x*x < 2 を満たす最大のxの近似 -> sqrt(2)に収束するはず
    double eps = 1e-9;
    double x = binary_search<double>(0.0, 2.0, [](double x) { return x * x < 2.0; }, eps);
    assert(std::abs(x - std::sqrt(2.0)) < 1e-6);

    //  単調減少述語（x < threshold）にも対応できることを確認
    double threshold = 3.14159;
    double y = binary_search<double>(-10.0, 10.0, [&](double x) { return x < threshold; }, eps);
    assert(std::abs(y - threshold) < 1e-6);
}

}  //  namespace nono

int main() {
    nono::solve();
}
