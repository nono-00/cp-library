#pragma once

#include <algorithm>
#include <limits>

namespace nono {

namespace monoid {

//  # Min
//  最小値。op(a, b) = min(a, b), e() = Tの最大値
template <class T>
struct Min {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::min(lhs, rhs);
    }
    static Value e() {
        return std::numeric_limits<T>::max();
    }
};

}  //  namespace monoid

}  //  namespace nono
