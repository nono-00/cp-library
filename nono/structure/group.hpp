#pragma once

namespace nono {


namespace group {

///  # Add
template <class T>
struct Add {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return lhs + rhs;
    }
    static Value e() {
        return Value{0};
    }
    static Value inv(Value value) {
        return -value;
    }
};

}  //  namespace group

}  //  namespace nono
