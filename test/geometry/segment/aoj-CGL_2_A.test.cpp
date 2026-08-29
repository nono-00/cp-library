// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/2/CGL_2_A
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
        if (parallel(s0, s1)) {
            std::cout << 2 << '\n';
        } else if (orthogonal(s0, s1)) {
            std::cout << 1 << '\n';
        } else {
            std::cout << 0 << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
