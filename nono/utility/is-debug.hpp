#pragma once

#include <cassert>

//  # DEBUG
//  重いassertを有効にするかどうかを決めるマクロ。未定義なら0(無効)になる
//  有効にするときは -DDEBUG=1 を付けてコンパイルする
#ifndef DEBUG
#define DEBUG 0
#endif

namespace nono {

//  # is_debug()
//  DEBUGが0以外ならtrue
constexpr bool is_debug() {
    return DEBUG;
}

}  //  namespace nono

//  # DEBUG_ASSERT(cond)
//  is_debug()がtrueのときだけ assert(cond) を行う。falseのときはcondを評価しない
//  入力全体をO(n)以上かけて検査するような重いassertに使う
#define DEBUG_ASSERT(cond)                            \
    do {                                              \
        if constexpr (nono::is_debug()) assert(cond); \
    } while (false)
