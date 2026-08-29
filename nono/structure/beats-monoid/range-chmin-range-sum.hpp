#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

///  # RangeChminRangeSum
template <class T, class Count = unsigned>
struct RangeChminRangeSum {
    struct Value {
        Value(): top2({std::numeric_limits<T>::min(), std::numeric_limits<T>::min()}), num(0), sum(0) {}
        Value(T val, Count num = 1): top2({val, std::numeric_limits<T>::min()}), num(num), sum(val) {}
        //  v[0]: maximum value, v[1]: second maximum value
        std::array<T, 2> top2;
        //  n: num of maximum value
        Count num;
        //  s: sum of segment
        T sum;
    };
    using Act = T;
    static Value op(Value lhs, Value rhs) {
        Value result;
        int l = 0, r = 0;
        result.sum = lhs.sum + rhs.sum;
        for (int i = 0; i < 2; i++) {
            if (lhs.top2[l] == rhs.top2[r]) {
                result.top2[i] = lhs.top2[l];
                if (i == 0) result.num = lhs.num + rhs.num;
                l++, r++;
            } else if (lhs.top2[l] < rhs.top2[r]) {
                result.top2[i] = rhs.top2[r];
                if (i == 0) result.num = rhs.num;
                r++;
            } else {
                result.top2[i] = lhs.top2[l];
                if (i == 0) result.num = lhs.num;
                l++;
            }
        }
        return result;
    }
    static Value e() {
        return Value{};
    }
    static std::optional<Value> mapping(Act act, Value value) {
        if (act >= value.top2[0]) {
            return value;
        } else if (act > value.top2[1]) {
            value.sum -= (value.top2[0] - act) * value.num;
            value.top2[0] = act;
            return value;
        } else {
            return std::nullopt;
        }
    }
    static Act composition(Act lhs, Act rhs) {
        return std::min(lhs, rhs);
    }
    static Act id() {
        return std::numeric_limits<T>::max();
    }
};

}  //  namespace beats_monoid

}  //  namespace nono
