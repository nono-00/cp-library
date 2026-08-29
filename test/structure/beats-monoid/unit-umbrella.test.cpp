// competitive-verifier: STANDALONE
//  beats-monoid.hppは個別の構造体ヘッダをまとめるアンブレラヘッダ。個々のロジックは
//  test/ds/segment-tree-beats/等で間接的に検証されるため、ここではアンブレラ経由の
//  includeがそのままコンパイルできることだけを確認する。
#include <cassert>

#include "nono/structure/beats-monoid.hpp"

namespace nono {

void solve() {
    using Monoid = beats_monoid::RangeChminRangeSum<int>;
    using Value = Monoid::Value;
    Value v(5);
    assert(v.sum == 5);
}

}  //  namespace nono

int main() {
    nono::solve();
}
