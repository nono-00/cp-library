#pragma once

#include <algorithm>
#include <optional>

namespace nono {

namespace act_monoid {

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

}  //  namespace act_monoid

}  //  namespace nono
