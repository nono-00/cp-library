#pragma once

namespace nono {

namespace act_monoid {

///  # Rev
template <class M>
struct Rev {
    using T = M::Value;
    struct Value {
        Value(): ord(M::e()), rev(M::e()) {}
        Value(T v): ord(v), rev(v) {}
        T ord, rev;
    };
    using Act = M::Act;
    static Value op(Value lhs, Value rhs) {
        lhs.ord = M::op(lhs.ord, rhs.ord);
        lhs.rev = M::op(rhs.rev, lhs.rev);
        return lhs;
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        value.ord = M::mapping(act, value.ord);
        value.rev = M::mapping(act, value.rev);
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        return M::composition(lhs, rhs);
    }
    static Act id() {
        return M::id();
    }
};

}  //  namespace act_monoid

}  //  namespace nono
