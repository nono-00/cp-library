#pragma once

#include <limits>

namespace nono {

namespace monoid {

//  # MinIndex
//  (a, i), (b, j)
//  [1] if a <= b:
//      return (a, i)
//  [2] else
//      return (b, j)
//  同じなら左側
//  indexが-1の値は単位元として扱う
template <class T>
struct MinIndex {
    struct Value {
        Value(T value, int index): value(value), index(index) {}
        T value;
        int index;
    };
    static Value op(Value lhs, Value rhs) {
        if (lhs.index == -1) return rhs;
        if (rhs.index == -1) return lhs;
        return (lhs.value <= rhs.value ? lhs : rhs);
    }
    static Value e() {
        return {std::numeric_limits<T>::max(), -1};
    }
};

}  //  namespace monoid

}  //  namespace nono
