// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/1/CGL_1_C
#include <iostream>

#include "nono/geometry/base.hpp"

namespace nono {

void solve() {
    Point p0, p1;
    std::cin >> p0 >> p1;
    int q;
    std::cin >> q;
    while (q--) {
        Point p2;
        std::cin >> p2;
        switch (ccw(p0, p1, p2)) {
            case 1:
                std::cout << "COUNTER_CLOCKWISE" << '\n';
                break;
            case -1:
                std::cout << "CLOCKWISE" << '\n';
                break;
            case 2:
                std::cout << "ONLINE_BACK" << '\n';
                break;
            case -2:
                std::cout << "ONLINE_FRONT" << '\n';
                break;
            case 0:
                std::cout << "ON_SEGMENT" << '\n';
                break;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
