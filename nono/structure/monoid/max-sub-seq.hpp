#pragma once

#include <algorithm>

namespace nono {

namespace monoid {

//  # MaxSubSeq
//  連続部分列の和の最大値。空の部分列(和0)も候補に含むので、0以上になる
//  max[for l in [0, n], for r in [l, n]](sum[for i in [l, r)](data[i]))
//  1要素は Value(v) で作り、結果は Value::max_subseq_sum() で取り出す
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
