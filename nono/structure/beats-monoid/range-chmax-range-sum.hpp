#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

//  # RangeChmaxRangeSum
//  区間chmax、区間和。Actはchmaxする値 (x <- max(x, act))
//  1要素は Value(val) で作る
//  mappingは、actが2番目に小さい値以上のとき nullopt を返す(子に降りて計算し直す)
template <class T, class Count = unsigned>
struct RangeChmaxRangeSum {
    struct Value {
        Value(): top2({std::numeric_limits<T>::max(), std::numeric_limits<T>::max()}), num(0), sum(0) {}
        Value(T val, Count num = 1): top2({val, std::numeric_limits<T>::max()}), num(num), sum(val) {}
        //  top2[0]: 最小値, top2[1]: 2番目に小さい値(最小値と異なるもの)
        std::array<T, 2> top2;
        //  最小値の個数
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
            } else if (lhs.top2[l] > rhs.top2[r]) {
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
        if (act <= value.top2[0]) {
            return value;
        } else if (act < value.top2[1]) {
            value.sum += (act - value.top2[0]) * value.num;
            value.top2[0] = act;
            return value;
        } else {
            return std::nullopt;
        }
    }
    static Act composition(Act lhs, Act rhs) {
        return std::max(lhs, rhs);
    }
    static Act id() {
        return std::numeric_limits<Act>::min();
    }
};

}  //  namespace beats_monoid

}  //  namespace nono
