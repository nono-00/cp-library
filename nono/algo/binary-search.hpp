#pragma once

#include <cassert>
#include <concepts>
#include <numeric>

/// brief : 二分探索
/// TODO : 浮動小数点数にぶたんのverify
namespace nono {

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
