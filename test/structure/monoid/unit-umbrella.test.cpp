// competitive-verifier: STANDALONE
//  monoid.hppは個別の構造体ヘッダをまとめるアンブレラヘッダ。個々のロジックは
//  各構造体のunit-*.test.cppで検証済みなので、ここではアンブレラ経由の
//  includeがそのままコンパイルできることだけを確認する。
#include <cassert>

#include "nono/structure/monoid.hpp"

namespace nono {

void solve() {
    using Monoid = monoid::Add<int>;
    assert(Monoid::op(2, 3) == 5);
    assert(Monoid::op(Monoid::e(), 7) == 7);
}

}  //  namespace nono

int main() {
    nono::solve();
}
