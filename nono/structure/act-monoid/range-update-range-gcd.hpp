#pragma once

#include <numeric>
#include <optional>

namespace nono {

namespace act_monoid {

//  # RangeUpdateRangeGcd
//  区間更新(代入)、区間gcd
//  Actは std::optional<T> で、値があればその値を代入し、nulloptなら何もしない
template <class T>
struct RangeUpdateRangeGcd {
    using Value = T;
    using Act = std::optional<T>;
    static Value op(Value lhs, Value rhs) {
        return std::gcd(lhs, rhs);
    }
    static Value e() {
        return Value{0};
    }
    static Value mapping(Act act, Value value) {
        if (!act) return value;
        return *act;
    }
    static Act composition(Act lhs, Act rhs) {
        if (!lhs) return rhs;
        return lhs;
    }
    static Act id() {
        return std::nullopt;
    }
};

}  //  namespace act_monoid

}  //  namespace nono
