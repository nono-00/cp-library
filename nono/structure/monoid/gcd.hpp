#pragma once

#include <numeric>

namespace nono {

namespace monoid {

//  # Gcd
//  最大公約数。op(a, b) = gcd(a, b), e() = 0
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
