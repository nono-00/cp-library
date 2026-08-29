#pragma once

namespace nono {

namespace monoid {

///  # Add
template <class T>
struct Add {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return lhs + rhs;
    }
    static Value e() {
        return static_cast<Value>(0);
    }
};

}  //  namespace monoid

}  //  namespace nono
