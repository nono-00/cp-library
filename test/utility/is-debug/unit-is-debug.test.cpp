// competitive-verifier: STANDALONE
#include <cassert>

#include "nono/utility/is-debug.hpp"

namespace nono {

//  テンプレート文脈でif constexpr (is_debug())を使っても
//  コンパイルできることを確認する
template <class T>
T maybe_heavy_check(T x) {
    if constexpr (is_debug()) {
        assert(x == x);
    }
    return x;
}

void solve() {
    //  -Dフラグなしでコンパイルした場合、デフォルトでfalse(本番相当)になる
    assert(is_debug() == false);

    //  非テンプレート文脈でのif constexpr使用
    if constexpr (is_debug()) {
        assert(false);  //  is_debug()==falseなので到達しない
    }

    assert(maybe_heavy_check(42) == 42);

    //  DEBUG_ASSERTはis_debug()==falseのとき何もしない
    DEBUG_ASSERT(false);
    int called = 0;
    auto side_effect = [&] {
        called++;
        return true;
    };
    DEBUG_ASSERT(side_effect());
    assert(called == 0);
}

}  //  namespace nono

int main() {
    nono::solve();
}
