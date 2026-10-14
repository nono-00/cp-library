#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

//  # RangeDivideUpdateRangeSum
//  区間除算(切り捨て)・区間更新(代入)、区間和。値は符号なし64bit整数
//  https://atcoder.jp/contests/abc256/tasks/abc256_h
//  作用は Act::divide(a) (x <- x / a), Act::update(b) (x <- b) で作る。1要素は Value(v) で作る
//  mappingは、区間内の値が揃っていない区間を割るとき nullopt を返す(子に降りて計算し直す)
struct RangeDivideUpdateRangeSum {
  private:
    using u64 = std::uint64_t;
    static constexpr u64 divide_id = 1;
    static constexpr u64 update_id = -1;

  public:
    struct Value {
        Value(): left(0), sum(0), num(0), same(true) {}
        Value(u64 v): left(v), sum(v), num(1), same(true) {}
        //  区間の左端の値
        u64 left;
        //  区間の和
        u64 sum;
        //  区間の長さ
        u64 num;
        //  区間内の値が全て等しいか
        bool same;
    };
    struct Act {
      private:
        //  divide_: 割る数。update_: 代入する値(無ければnullopt)
        u64 divide_;
        std::optional<u64> update_;
        friend struct RangeDivideUpdateRangeSum;

      public:
        Act(u64 a, std::optional<u64> b): divide_(a), update_(b) {}
        static Act divide(u64 a) {
            return Act{a, std::nullopt};
        };
        static Act update(u64 b) {
            return Act{1, b};
        };
    };
    static Value op(Value lhs, Value rhs) {
        Value result;
        result.left = lhs.left;
        result.sum = lhs.sum + rhs.sum;
        result.num = lhs.num + rhs.num;
        result.same = lhs.same && rhs.same && lhs.left == rhs.left;
        return result;
    }
    static Value e() {
        return Value{};
    }
    static std::optional<Value> mapping(Act act, Value value) {
        if (!value.same && !act.update_ && act.divide_ != 1) {
            return std::nullopt;
        }
        if (act.update_) {
            value.same = true;
            value.left = act.update_.value();
        }
        value.left /= act.divide_;
        if (value.same) {
            value.sum = value.left * value.num;
        }
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        if (lhs.update_) return lhs;
        return Act{std::min(lhs.divide_ * rhs.divide_, (u64)std::numeric_limits<unsigned>::max()), rhs.update_};
    }
    static Act id() {
        return Act{1, std::nullopt};
    }
};

}  //  namespace beats_monoid

}  //  namespace nono
