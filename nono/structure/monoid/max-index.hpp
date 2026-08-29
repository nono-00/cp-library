#pragma once

#include <limits>

namespace nono {

namespace monoid {

///  # MaxIndex
template <class T>
struct MaxIndex {
    struct Value {
        T value;
        int index;
    };
    static Value op(Value lhs, Value rhs) {
        if (lhs.index == -1) return rhs;
        if (rhs.index == -1) return lhs;
        return (lhs.value >= rhs.value ? lhs : rhs);
    }
    static Value e() {
        return {std::numeric_limits<T>::min(), -1};
    }
};

}  //  namespace monoid

}  //  namespace nono
