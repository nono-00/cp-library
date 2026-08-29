// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/1/CGL_1_A
// competitive-verifier: ERROR 1e-8
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/segment.hpp"

namespace nono {

void solve() {
    Point p1, p2;
    std::cin >> p1 >> p2;
    Segment s(p1, p2);
    int q;
    std::cin >> q;
    while (q--) {
        Point p;
        std::cin >> p;
        Point proj = projection(s, p);
        std::cout << proj.x << ' ' << proj.y << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
