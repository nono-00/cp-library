// competitive-verifier: STANDALONE
#include <cassert>

#include "nono/geometry/base.hpp"

namespace nono {

void solve() {
    //  p0=(1,0), p1=(0,0), p2=(0,1) -> 90度 = PI/2
    assert(eq(angle(Point(1, 0), Point(0, 0), Point(0, 1)), PI / 2));

    //  p0=(1,0), p1=(0,0), p2=(-1,0) -> 180度 = PI
    assert(eq(angle(Point(1, 0), Point(0, 0), Point(-1, 0)), PI));

    //  p0=(1,0), p1=(0,0), p2=(1,1) -> 45度 = PI/4
    assert(eq(angle(Point(1, 0), Point(0, 0), Point(1, 1)), PI / 4));

    //  p0とp2を入れ替えても同じ角度
    assert(eq(angle(Point(0, 1), Point(0, 0), Point(1, 0)), PI / 2));

    //  gte(lhs, rhs): lhs >= rhs
    assert(gte(1.0, 1.0));
    assert(gte(2.0, 1.0));
    assert(!gte(1.0, 2.0));
}

}  //  namespace nono

int main() {
    nono::solve();
}
