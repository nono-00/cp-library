#pragma once

#include <algorithm>
#include <limits>

namespace nono {

namespace monoid {

//  # MinMax
//  最小値と最大値の組。op は、minどうしの小さいほうとmaxどうしの大きいほうを取る
//  1要素は Value(val) で作る
//  Tが浮動小数点数だと壊れる(numeric_limits<T>::min() が正の最小値になるため)
template <class T>
struct MinMax {
    struct Value {
        Value(): min(std::numeric_limits<T>::max()), max(std::numeric_limits<T>::min()) {}
        Value(T val): min(val), max(val) {}
        Value(T min, T max): min(min), max(max) {}
        T min, max;
    };
    static Value op(Value lhs, Value rhs) {
        return Value{std::min(lhs.min, rhs.min), std::max(lhs.max, rhs.max)};
    }
    static Value e() {
        return Value{};
    }
};

}  //  namespace monoid

}  //  namespace nono
