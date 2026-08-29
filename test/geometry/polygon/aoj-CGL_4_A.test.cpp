// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/library/4/CGL/4/CGL_4_A
#include <algorithm>
#include <iostream>

#include "nono/geometry/base.hpp"
#include "nono/geometry/polygon.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    Points points(n);
    for (auto& p: points) std::cin >> p;
    Polygon hull = convex_hull(points);
    int m = hull.size();
    //  AOJの出力仕様: y最小(tieはx最小)の頂点から反時計回りに出力する
    int start = 0;
    for (int i = 1; i < m; i++) {
        if (hull[i].y < hull[start].y || (hull[i].y == hull[start].y && hull[i].x < hull[start].x)) {
            start = i;
        }
    }
    std::cout << m << '\n';
    for (int i = 0; i < m; i++) {
        const Point& p = hull[(start + i) % m];
        std::cout << (long long)p.x << ' ' << (long long)p.y << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
