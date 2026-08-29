// competitive-verifier: STANDALONE
//  act-monoid.hppは個別の構造体ヘッダをまとめるアンブレラヘッダ。個々のロジックは
//  各構造体のunit-*.test.cppで検証済みなので、ここではアンブレラ経由の
//  includeがそのままコンパイルできることだけを確認する。
#include <cassert>

#include "nono/structure/act-monoid.hpp"

namespace nono {

void solve() {
    using Monoid = act_monoid::RangeAffineRangeSum<int>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;
    Value v(3);
    Act act(2, 1);  //  x <- 2x + 1
    assert(Monoid::mapping(act, v).sum == 7);
}

}  //  namespace nono

int main() {
    nono::solve();
}
