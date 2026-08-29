#pragma once

namespace nono {

namespace monoid {

///  # Composite
///  (ax + b) 関数の合成
template <class T>
struct Composite {
    struct Value {
        Value(T a = 1, T b = 0): a(a), b(b) {}
        T a;
        T b;
        T eval(T x) {
            return a * x + b;
        }
    };
    static Value op(Value lhs, Value rhs) {
        return {lhs.a * rhs.a, rhs.a * lhs.b + rhs.b};
    }
    static Value e() {
        return Value{};
    }
};

}  //  namespace monoid

}  //  namespace nono
