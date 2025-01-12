#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <optional>

namespace nono {

namespace monoid {

struct MonoidTemplate {
    struct Value {};
    static Value op(Value lhs, Value rhs);
    static Value e();
};

///  # Min
template <class T>
struct Min {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::min(lhs, rhs);
    }
    static Value e() {
        return std::numeric_limits<T>::max();
    }
};

///  # Max
template <class T>
struct Max {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return std::max(lhs, rhs);
    }
    static Value e() {
        return std::numeric_limits<T>::min();
    }
};

///  # MinMax
template <class T>
struct MinMax {
    struct Value {
        Value(): min(std::numeric_limits<T>::max()), max(std::numeric_limits<T>::min()) {}
        Value(T val): min(val), max(val) {}
        Value(T min, T max): min(min), max(max) {}
        T min, max;
    };
    static Value op(Value lhs, Value rhs) {
        return Value{std::min(lhs.min, rhs.min), std::max(lhs.max, rhs.max)};
    }
    static Value e() {
        return Value{};
    }
};

///  # Add
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

///  # Mul
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

///  # MinIndex
///  (a, i), (b, j)
///  [1] if a < b:
///      return (a, i)
///  [2] else
///      return (b, j)
///  同じなら左側
template <class T>
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

///  # MaxIndex
///  [1] if a > b:
///      return (a, i)
///  [2] else
///      return (b, j)
///  同じなら左側
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

///  # Rev
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

///  # Gcd
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

///  # MaxSubSeq
///  max[for r in [0, n), for r in [l + 1, n]](sum[for i in [l, r)](data[i]))
template <class T>
struct MaxSubSeq {
    struct Value {
        Value(T v = 0): val(std::max(T{0}, v)), prefix(std::min(T{0}, v)), suffix(std::max(T{0}, v)), sum(v) {}
        T max_subseq_sum() const {
            return val;
        }
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
