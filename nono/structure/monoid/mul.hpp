#pragma once

namespace nono {

namespace monoid {

//  # Mul
//  積。op(a, b) = a * b, e() = 1
template <class T>
struct Mul {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return lhs * rhs;
    }
    static Value e() {
        return static_cast<Value>(1);
    }
};

}  //  namespace monoid

}  //  namespace nono
