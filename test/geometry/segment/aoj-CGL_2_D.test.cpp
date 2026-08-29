// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/2/CGL_2_D
// competitive-verifier: ERROR 1e-8
#include <iomanip>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/segment.hpp"

namespace nono {

void solve() {
    int q;
    std::cin >> q;
    while (q--) {
        Point p0, p1, p2, p3;
        std::cin >> p0 >> p1 >> p2 >> p3;
        Segment s0(p0, p1), s1(p2, p3);
        std::cout << distance(s0, s1) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(10);
    nono::solve();
}
