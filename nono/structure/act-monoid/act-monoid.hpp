#pragma once

#include <variant>

namespace nono {

namespace act_monoid {

///  # ActMonoid
template <class M>
struct ActMonoid {
    using Value = M::Value;
    using Act = std::monostate;
    static Value op(Value lhs, Value rhs) {
        return M::op(lhs, rhs);
    }
    static Value e() {
        return M::e();
    }
    static Value mapping(Act, Value value) {
        return value;
    }
    static Act composition(Act, Act) {
        return {};
    }
    static Act id() {
        return {};
    }
};

}  //  namespace act_monoid

}  //  namespace nono
