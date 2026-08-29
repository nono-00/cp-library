// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/7/CGL_7_E
// competitive-verifier: ERROR 1e-6
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/circle.hpp"

namespace nono {

void solve() {
    Real c1x, c1y, c1r, c2x, c2y, c2r;
    std::cin >> c1x >> c1y >> c1r >> c2x >> c2y >> c2r;
    Circle c1(c1r, Point(c1x, c1y)), c2(c2r, Point(c2x, c2y));
    Points cps = crosspoint(c1, c2);
    if (cps.size() == 1) cps.push_back(cps[0]);
    std::sort(cps.begin(), cps.end());
    std::cout << cps[0].x << ' ' << cps[0].y << ' ' << cps[1].x << ' ' << cps[1].y << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
