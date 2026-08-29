#pragma once

#include <cmath>
#include <numeric>

namespace nono {

namespace act_monoid {

///  # RangeAddRangeGcd
///    gcd(A_0, A_1, A_2, ...)
///  = gcd(A_0, A_1 - A_0, A_2 - A_1, ...)
///
///  tot := gcd(A_1 - A_0, A_2 - A_1, ...)
///  answer := gcd(A_0, tot)
///
///  op lhs rhs :=
///     left <- lhs.left,
///     right <- rhs.right,
///     tot <- gcd(left.tot, right.tot, lhs.right - rhs.left)
///
//// ## range add
///  mapping act value :=
///     left <- left + act,
///     right <- right + act,
///     tot <- tot
///     (totは差分のgcd, すべての値にact足しても差分は不変)
template <class T>
struct RangeAddRangeGcd {
    struct Value {
        //  emptyはe()（空区間）を表す。left/right/totに実データと衝突しない
        //  番兵値が存在しないため、フラグで明示的に区別する
        Value(): left(0), right(0), tot(0), empty(true) {}
        Value(T v): left(v), right(v), tot(0), empty(false) {}
        T left = 0;
        T right = 0;
        T tot = 0;
        bool empty = false;
        T val() {
            return std::gcd(tot, left);
        }
    };
    using Act = T;
    static Value op(Value lhs, Value rhs) {
        if (lhs.empty) return rhs;
        if (rhs.empty) return lhs;
        Value res{0};
        res.left = lhs.left;
        res.right = rhs.right;
        res.tot = std::gcd(lhs.tot, std::gcd(rhs.tot, std::abs(lhs.right - rhs.left)));
        return res;
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        if (value.empty) return value;
        value.left += act;
        value.right += act;
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        return lhs + rhs;
    }
    static Act id() {
        return Act{0};
    }
};

}  //  namespace act_monoid

}  //  namespace nono
