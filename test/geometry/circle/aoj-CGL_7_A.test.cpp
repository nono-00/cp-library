// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/7/CGL_7_A
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/circle.hpp"

namespace nono {

void solve() {
    Real c1x, c1y, c1r, c2x, c2y, c2r;
    std::cin >> c1x >> c1y >> c1r >> c2x >> c2y >> c2r;
    Circle c1(c1r, Point(c1x, c1y)), c2(c2r, Point(c2x, c2y));
    std::cout << intersect(c1, c2) << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
