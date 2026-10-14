#pragma once

#include <optional>

namespace nono {

namespace act_monoid {

//  # RangeAddCountMax
//  区間加算、区間に含まれる最大値とその出現回数
//  op      : 最大値が大きいほうを残す。等しければ出現回数を足す
//  mapping : 最大値にactを足す。出現回数は変わらない
//  1要素は Value(val) で作る
template <class T, class Count = unsigned>
struct RangeAddCountMax {
    //  (max, count)
    struct Value {
        Value(): max(std::nullopt), num(0) {}
        Value(std::optional<T> val, Count num = 1): max(val), num(num) {}
        std::optional<T> max;
        Count num;
    };
    using Act = T;
    static Value op(Value lhs, Value rhs) {
        if (!lhs.max) return rhs;
        if (!rhs.max) return lhs;
        if (lhs.max == rhs.max) return Value{lhs.max, lhs.num + rhs.num};
        return lhs.max > rhs.max ? lhs : rhs;
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        if (value.max) return Value{*value.max + act, value.num};
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
