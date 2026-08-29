#pragma once

#include <numeric>

namespace nono {

namespace monoid {

///  # Gcd
template <class T>
struct Gcd {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::gcd(lhs, rhs);
    }
    static Value e() {
        return Value{0};
    }
};

}  //  namespace monoid

}  //  namespace nono
