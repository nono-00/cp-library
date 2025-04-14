#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <optional>
#include <variant>

namespace nono {

namespace act_monoid {

template <class T>
struct ActMonoidTemplate {
    struct Value {};
    struct Act {};
    static Value op(Value lhs, Value rhs);
    static Value e();
    static Value mapping(Act act, Value value);
    ///  lhs(rhs(x))
    static Act compososition(Act lhs, Act rhs);
    static Act id();
};

///  # RangeAffineRangeSum
///  op a b := a + b
///  mapping (a, b) x := x <- ax + b
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

///  # RangeAddCountMin
///  区間に含まれる最小値とその出現回数
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

///  # RangeAddCountMin
///  区間に含まれる最大値とその出現回数
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
        Value(T v = 0): left(v), right(v), tot(0) {}
        T left = 0;
        T right = 0;
        T tot = 0;
        T val() {
            return std::gcd(tot, left);
        }
    };
    using Act = T;
    static Value op(Value lhs, Value rhs) {
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

///  # RangeUpdateRangeGcd
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

///  # RangeUpdateMaxSubSeq
///  任意の連続部分列の総和の最大値
template <class T, class Count = unsigned>
struct RangeUpdateMaxSubSeq {
    struct Value {
        Value(T v = 0, Count n = 1)
            : val(std::max(T{0}, v)),
              prefix(std::min(T{0}, v)),
              suffix(std::max(T{0}, v)),
              sum(v),
              num(n) {}
        T val, prefix, suffix, sum;
        Count num;
    };
    using Act = std::optional<T>;

    static Value op(Value lhs, Value rhs) {
        Value result{};
        result.prefix = std::min(lhs.prefix, lhs.sum + rhs.prefix);
        result.suffix = std::max(lhs.suffix, lhs.sum + rhs.suffix);
        result.sum = lhs.sum + rhs.sum;
        result.val = std::max({lhs.val, rhs.val, (rhs.suffix + lhs.sum) - (lhs.prefix)});
        result.num = lhs.num + rhs.num;
        return result;
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        if (!act) return value;
        if (*act <= 0) {
            value.val = 0;
            value.prefix = value.sum = *act * value.num;
            value.suffix = 0;
        } else {
            value.val = value.suffix = value.sum = *act * value.num;
            value.prefix = 0;
        }
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        if (lhs) return lhs;
        return rhs;
    }
    static Act id() {
        return std::nullopt;
    }
};

///  # RangeChminChmaxAddRangeMinMax
///  Act (low, high, add) (x) -> min(high, max(low, x + add))
template <class T>
struct RangeChminChmaxAddRangeMinMax {
  private:
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();

  public:
    struct Value {
        Value(): min(MAX), max(MIN) {}
        Value(T value): min(value), max(value) {}
        Value(T min, T max): min(min), max(max) {}
        T min;
        T max;
    };
    struct Act {
        T chmin_;
        T chmax_;
        T add_;
        static Act chmin(T a) {
            return Act{a, MIN, static_cast<T>(0)};
        }
        static Act chmax(T a) {
            return Act{MAX, a, static_cast<T>(0)};
        }
        static Act clamp(T low, T high) {
            return Act{high, low, static_cast<T>(0)};
        }
        static Act add(T b) {
            return Act{MAX, MIN, b};
        }
        Act(T a, T b, T c): chmin_(a), chmax_(b), add_(c) {}
    };
    static Value op(Value lhs, Value rhs) {
        return Value{std::min(lhs.min, rhs.min), std::max(lhs.max, rhs.max)};
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        return Value{
            std::clamp(value.min + act.add_, act.chmax_, act.chmin_),
            std::clamp(value.max + act.add_, act.chmax_, act.chmin_),
        };
    }
    static Act composition(Act lhs, Act rhs) {
        T chmin = (lhs.chmin_ != MAX ? lhs.chmin_ - rhs.add_ : lhs.chmin_);
        T chmax = (lhs.chmax_ != MIN ? lhs.chmax_ - rhs.add_ : lhs.chmax_);
        if (rhs.chmin_ <= chmax) return Act{chmax, chmax, lhs.add_ + rhs.add_};
        if (chmin <= rhs.chmax_) return Act{chmin, chmin, lhs.add_ + rhs.add_};
        return Act{std::min(rhs.chmin_, chmin), std::max(rhs.chmax_, chmax), lhs.add_ + rhs.add_};
    }
    static Act id() {
        return Act{MAX, MIN, static_cast<T>(0)};
    }
};

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
