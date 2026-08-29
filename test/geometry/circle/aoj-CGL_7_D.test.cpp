// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/7/CGL_7_D
// competitive-verifier: ERROR 1e-6
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/circle.hpp"

namespace nono {

void solve() {
    Real cx, cy, r;
    std::cin >> cx >> cy >> r;
    Circle c(r, Point(cx, cy));
    int q;
    std::cin >> q;
    while (q--) {
        Point p1, p2;
        std::cin >> p1 >> p2;
        Points cps = crosspoint(c, Line(p1, p2));
        if (cps.size() == 1) cps.push_back(cps[0]);
        std::sort(cps.begin(), cps.end());
        std::cout << cps[0].x << ' ' << cps[0].y << ' ' << cps[1].x << ' ' << cps[1].y << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
