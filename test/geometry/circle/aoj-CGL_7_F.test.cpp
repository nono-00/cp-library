// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/7/CGL_7_F
// competitive-verifier: ERROR 1e-5
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/circle.hpp"

namespace nono {

void solve() {
    Real px, py, cx, cy, r;
    std::cin >> px >> py >> cx >> cy >> r;
    Circle c(r, Point(cx, cy));
    Points tps = tangent(c, Point(px, py));
    std::sort(tps.begin(), tps.end());
    std::cout << tps[0].x << ' ' << tps[0].y << '\n';
    std::cout << tps[1].x << ' ' << tps[1].y << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
