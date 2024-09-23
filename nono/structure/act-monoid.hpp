#pragma once

#include <cmath>
#include <numeric>
#include <optional>

///  brief : ActMonoid全部盛り.
namespace nono {
namespace act_monoid {
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
        if (lhs.min == rhs.min) return Value{lhs.min, lhs.num + rhs.num};
        if (!lhs.min) return rhs;
        if (!rhs.min) return lhs;
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
template <class T, class Count = unsigned>
struct RangeAddCountMax {
    //  (min, count)
    struct Value {
        Value(): max(std::nullopt), num(0) {}
        Value(std::optional<T> val, Count num = 1): max(val), num(num) {}
        std::optional<T> max;
        Count num;
    };
    using Act = T;
    static Value op(Value lhs, Value rhs) {
        if (lhs.max == rhs.max) return Value{lhs.max, lhs.num + rhs.num};
        if (!lhs.max) return rhs;
        if (!rhs.max) return lhs;
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
template <class T>
struct RangeAddRangeGcd {
  private:
    struct Value_ {
        Value_(T v = 0): left(v), right(v), tot(0) {}
        T left = 0;
        T right = 0;
        T tot = 0;
        T val() {
            return std::gcd(tot, left);
        }
    };

  public:
    using Value = std::optional<Value_>;
    using Act = T;
    static Value op(Value lhs, Value rhs) {
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        Value res{0};
        res->left = lhs->left;
        res->right = rhs->right;
        res->tot = std::gcd(lhs->tot, std::gcd(rhs->tot, std::abs(lhs->right - rhs->left)));
        return res;
    }
    static Value e() {
        return std::nullopt;
    }
    static Value mapping(Act act, Value value) {
        value->left += act;
        value->right += act;
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        return lhs + rhs;
    }
    static Act id() {
        return Act{0};
    }
};
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
