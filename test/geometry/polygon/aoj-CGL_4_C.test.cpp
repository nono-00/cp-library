// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/4/CGL_4_C
// competitive-verifier: ERROR 1e-5
#include <cmath>
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/polygon.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    Polygon g(n);
    for (auto& p: g) std::cin >> p;
    int q;
    std::cin >> q;
    while (q--) {
        Point p1, p2;
        std::cin >> p1 >> p2;
        Polygon cut = convex_cut(g, Line(p1, p2));
        std::cout << std::abs(area(cut)) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
