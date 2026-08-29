// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>

#include "nono/geometry/base.hpp"
#include "nono/geometry/polygon.hpp"

namespace nono {

void solve() {
    //  perimeter: 1x1の正方形
    Polygon square = {Point(0, 0), Point(1, 0), Point(1, 1), Point(0, 1)};
    assert(eq(perimeter(square), 4));

    //  contain(Polygon, Segment)
    assert(contain(square, Segment(Point(0.2, 0.2), Point(0.8, 0.8))));
    assert(!contain(square, Segment(Point(0.5, 0.5), Point(2, 2))));
    //  境界を通る辺も内包とみなす
    assert(contain(square, Segment(Point(0, 0), Point(1, 1))));

    //  lower_and_upper_convex_hull: 正方形の下側/上側の分割
    auto [lower, upper] = lower_and_upper_convex_hull(square);
    assert(lower.front() == upper.front());
    assert(lower.back() == upper.back());
    assert(lower.front() == Point(0, 0));
    assert(lower.back() == Point(1, 1));

    //  common_polygon / intersect(Polygon, Polygon):
    //  [0,1]x[0,1]の正方形と[0.5,1.5]x[0.5,1.5]の正方形の共通部分は[0.5,1]x[0.5,1]の正方形(面積0.25)
    Polygon shifted = {Point(0.5, 0.5), Point(1.5, 0.5), Point(1.5, 1.5), Point(0.5, 1.5)};
    Polygon common = common_polygon(square, shifted);
    assert(eq(std::abs(area(common)), 0.25));
    assert(intersect(square, shifted));

    //  重ならない2つの正方形
    Polygon far = {Point(10, 10), Point(11, 10), Point(11, 11), Point(10, 11)};
    assert(!intersect(square, far));
    assert(area(common_polygon(square, far)) == 0);
}

}  //  namespace nono

int main() {
    nono::solve();
}
