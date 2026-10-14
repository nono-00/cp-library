#pragma once

#include <algorithm>
#include <limits>

namespace nono {

namespace monoid {

//  # Max
//  最大値。op(a, b) = max(a, b), e() = Tの最小値
//  Tが浮動小数点数だと壊れる(numeric_limits<T>::min() が正の最小値になるため)
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
