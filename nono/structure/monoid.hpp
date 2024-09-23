#pragma once

#include <algorithm>
#include <numeric>
#include <optional>

///  brief : Monoid全部盛り.
namespace nono {
namespace monoid {
template <class T>
struct Min {
    using Value = std::optional<T>;
    static Value op(Value lhs, Value rhs) {
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        return std::min(lhs, rhs);
    }
    static Value e() {
        return std::nullopt;
    }
};
template <class T>
struct Max {
    using Value = std::optional<T>;
    static Value op(Value lhs, Value rhs) {
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        return std::max(lhs, rhs);
    }
    static Value e() {
        return std::nullopt;
    }
};
template <class T>
struct Add {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return lhs + rhs;
    }
    static Value e() {
        return static_cast<Value>(0);
    }
};
template <class T>
struct Mul {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return lhs * rhs;
    }
    static Value e() {
        return static_cast<Value>(1);
    }
};
//   ax+b
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
template <class T, class Index = unsigned>
struct MinIndex {
  private:
    struct Value_ {
        Value_(T val, int i): min(val), index(i) {}
        T min;
        int index;
    };

  public:
    using Value = std::optional<Value_>;
    static Value op(Value lhs, Value rhs) {
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        return lhs->min < rhs->min ? lhs : rhs;
    }
    static Value e() {
        return std::nullopt;
    }
};
template <class T>
struct MaxIndex {
  private:
    struct Value_ {
        Value_(T val, int i): max(val), index(i) {}
        T max;
        int index;
    };

  public:
    using Value = std::optional<Value_>;
    static Value op(Value lhs, Value rhs) {
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        return lhs->max > rhs->max ? lhs : rhs;
    }
    static Value e() {
        return std::nullopt;
    }
};
template <class M>
struct Rev {
    using T = M::Value;
    struct Value {
        Value(): ord(M::e()), rev(M::e()) {}
        Value(T v_): ord(v_), rev(v_) {}
        T ord, rev;
    };
    static Value op(Value lhs, Value rhs) {
        lhs.ord = M::op(lhs.ord, rhs.ord);
        lhs.rev = M::op(rhs.rev, lhs.rev);
        return lhs;
    }
    static Value e() {
        return Value{};
    }
};
template <class T>
struct Gcd {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::gcd(lhs, rhs);
    }
    static Value e() {
        return Value{0};
    }
};
//  任意の連続部分列の総和の最大値
template <class T>
struct MaxSubSeq {
    struct Value {
        Value(T v = 0): val(std::max(T{0}, v)), prefix(std::min(T{0}, v)), suffix(std::max(T{0}, v)), sum(v) {}
        T val, prefix, suffix, sum;
    };
    static Value op(Value lhs, Value rhs) {
        Value result{};
        result.prefix = std::min(lhs.prefix, lhs.sum + rhs.prefix);
        result.suffix = std::max(lhs.suffix, lhs.sum + rhs.suffix);
        result.sum = lhs.sum + rhs.sum;
        result.val = std::max({lhs.val, rhs.val, (rhs.suffix + lhs.sum) - (lhs.prefix)});
        return result;
    }
    static Value e() {
        return Value{};
    }
};
}  //  namespace monoid
}  //  namespace nono
