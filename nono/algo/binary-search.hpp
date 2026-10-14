#pragma once

#include <cassert>
#include <concepts>
#include <numeric>

namespace nono {

//  # binary_search(ok, ng, pred)
//  二分探索(整数)。pred(x - 1) = true かつ pred(x) = false となる x を返す
//  predが単調(ある値より小さいと常にtrue、それ以上で常にfalse)なら、predがfalseになる最小の値
//  ok < ng, pred(ok) = true, pred(ng) = false でないと壊れる
//  predを O(log (ng - ok)) 回呼ぶ
template <std::integral T, class F>
T binary_search(T ok, T ng, F&& pred) {
    assert(ok < ng);
    assert(pred(ok));
    assert(!pred(ng));
    while (ng - ok > 1) {
        T key = std::midpoint(ok, ng);
        (pred(key) ? ok : ng) = key;
    }
    return ng;
}

//  # binary_search(ok, ng, pred, eps)
//  二分探索(浮動小数点数)。pred(lo) = true, pred(x) = false, x - lo <= eps となるまで区間を狭め、x を返す
//  ok < ng, pred(ok) = true, pred(ng) = false でないと壊れる
//  epsが小さすぎて区間の中点が端と等しくなると終了しない
//  predを O(log ((ng - ok) / eps)) 回呼ぶ
template <std::floating_point T, class F>
T binary_search(T ok, T ng, F&& pred, const T eps) {
    assert(ok < ng);
    assert(pred(ok));
    assert(!pred(ng));
    while (ng - ok > eps) {
        T key = std::midpoint(ok, ng);
        (pred(key) ? ok : ng) = key;
    }
    return ng;
}

}  //  namespace nono
