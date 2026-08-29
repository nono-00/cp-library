// competitive-verifier: STANDALONE
#include <cassert>

#include "nono/geometry/base.hpp"
#include "nono/geometry/circle.hpp"

namespace nono {

void solve() {
    Circle c(5, Point(0, 0));

    //  中心
    assert(contain(c, Point(0, 0)));
    assert(strictly_contain(c, Point(0, 0)));
    assert(!intersect(c, Point(0, 0)));

    //  円周上
    assert(contain(c, Point(5, 0)));
    assert(contain(c, Point(0, 5)));
    assert(contain(c, Point(3, 4)));
    assert(!strictly_contain(c, Point(5, 0)));
    assert(intersect(c, Point(5, 0)));
    assert(intersect(c, Point(3, 4)));

    //  外部
    assert(!contain(c, Point(6, 0)));
    assert(!strictly_contain(c, Point(6, 0)));
    assert(!intersect(c, Point(6, 0)));

    //  円と線分
    assert(intersect(c, Segment(Point(-10, 0), Point(10, 0))));
    assert(intersect(c, Segment(Point(0, 0), Point(0, 3))));
    assert(!intersect(c, Segment(Point(10, 10), Point(20, 20))));

    //  面積 (r=5 -> 25*PI)
    assert(eq(area(c), 25 * PI));

    Circle unit(1, Point(0, 0));
    assert(eq(area(unit), PI));
}

}  //  namespace nono

int main() {
    nono::solve();
}
