// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/7/CGL_7_C
// competitive-verifier: ERROR 1e-6
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/circle.hpp"

namespace nono {

void solve() {
    Point p0, p1, p2;
    std::cin >> p0 >> p1 >> p2;
    Circle c = circumscribed_circle(p0, p1, p2);
    std::cout << c.c.x << ' ' << c.c.y << ' ' << c.r << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
