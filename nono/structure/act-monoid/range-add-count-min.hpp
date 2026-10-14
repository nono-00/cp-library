#pragma once

#include <optional>

namespace nono {

namespace act_monoid {

//  # RangeAddCountMin
//  区間加算、区間に含まれる最小値とその出現回数
//  op      : 最小値が小さいほうを残す。等しければ出現回数を足す
//  mapping : 最小値にactを足す。出現回数は変わらない
//  1要素は Value(val) で作る
template <class T, class Count = unsigned>
struct RangeAddCountMin {
    //  (min, count)
    struct Value {
        Value(): min(std::nullopt), num(0) {}
        Value(std::optional<T> val, Count num = 1): min(val), num(num) {}
        std::optional<T> min;
        Count num;
    };
    using Act = T;
    static Value op(Value lhs, Value rhs) {
        if (!lhs.min) return rhs;
        if (!rhs.min) return lhs;
        if (lhs.min == rhs.min) return Value{lhs.min, lhs.num + rhs.num};
        return lhs.min < rhs.min ? lhs : rhs;
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        if (value.min) return Value{*value.min + act, value.num};
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        return lhs + rhs;
    }
    static Act id() {
        return 0;
    }
};

}  //  namespace act_monoid

}  //  namespace nono
