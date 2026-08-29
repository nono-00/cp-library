#pragma once

#include <algorithm>
#include <limits>

namespace nono {

namespace monoid {

///  # Max
template <class T>
struct Max {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::max(lhs, rhs);
    }
    static Value e() {
        return std::numeric_limits<T>::min();
    }
};

}  //  namespace monoid

}  //  namespace nono
