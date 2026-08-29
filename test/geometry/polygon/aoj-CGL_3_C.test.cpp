// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/3/CGL_3_C
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/polygon.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    Polygon poly(n);
    for (auto& p: poly) std::cin >> p;
    int q;
    std::cin >> q;
    while (q--) {
        Point p;
        std::cin >> p;
        std::cout << contain(poly, p) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
