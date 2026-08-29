// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/7/CGL_7_G
// competitive-verifier: ERROR 1e-5
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
    auto lines = tangent(c1, c2);
    //  tangent(c1, c2)は内部でc1/c2を半径の大小でswapすることがあるため、
    //  返ってきた各Lineのどちらの端点が「入力時のc1」上の接点かをここで判定する
    Points points;
    for (auto& l: lines) {
        points.push_back(intersect(c1, l.p0) ? l.p0 : l.p1);
    }
    std::sort(points.begin(), points.end());
    for (auto& p: points) {
        std::cout << p.x << ' ' << p.y << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
