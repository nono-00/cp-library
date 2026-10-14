#pragma once

namespace nono {

namespace act_monoid {

//  # RangeAffineRangeSum
//  区間アフィン変換、区間和
//  op a b := a + b
//  mapping (a, b) x := x <- ax + b
//  1要素は Value(x) で作る。作用は Act(a, b)
template <class T, class Count = unsigned>
struct RangeAffineRangeSum {
    struct Value {
        Value(): sum(0), num(0) {}
        Value(T sum, Count num = 1): sum(sum), num(num) {}
        T sum;
        Count num;
    };
    //  (a, b): ax + b
    struct Act {
        Act(T a = 1, T b = 0): a(a), b(b) {}
        T a;
        friend bool operator==(Act lhs, Act rhs) {
            return lhs.a == rhs.a && lhs.b == rhs.b;
        }
        friend bool operator!=(Act lhs, Act rhs) {
            return !(lhs == rhs);
        }
        T b;
    };
    static Value op(Value lhs, Value rhs) {
        return Value{lhs.sum + rhs.sum, lhs.num + rhs.num};
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        return Value{act.a * value.sum + act.b * value.num, value.num};
    }
    static Act composition(Act lhs, Act rhs) {
        return Act{rhs.a * lhs.a, lhs.a * rhs.b + lhs.b};
    }
    static Act id() {
        return Act{};
    }
};

}  //  namespace act_monoid

}  //  namespace nono
