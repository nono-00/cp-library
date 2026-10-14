#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

//  # RangeChminRangeSum
//  区間chmin、区間和。Actはchminする値 (x <- min(x, act))
//  1要素は Value(val) で作る
//  mappingは、actが2番目に大きい値以下のとき nullopt を返す(子に降りて計算し直す)
template <class T, class Count = unsigned>
struct RangeChminRangeSum {
    struct Value {
        Value(): top2({std::numeric_limits<T>::min(), std::numeric_limits<T>::min()}), num(0), sum(0) {}
        Value(T val, Count num = 1): top2({val, std::numeric_limits<T>::min()}), num(num), sum(val) {}
        //  top2[0]: 最大値, top2[1]: 2番目に大きい値(最大値と異なるもの)
        std::array<T, 2> top2;
        //  最大値の個数
        Count num;
        //  区間の和
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
