#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

///  # RangeDivideUpdateRangeSum
///  https://atcoder.jp/contests/abc256/tasks/abc256_h
struct RangeDivideUpdateRangeSum {
  private:
    using u64 = std::uint64_t;
    static constexpr u64 divide_id = 1;
    static constexpr u64 update_id = -1;

  public:
    struct Value {
        Value(): left(0), sum(0), num(0), same(true) {}
        Value(u64 v): left(v), sum(v), num(1), same(true) {}
        //  left: most left value
        u64 left;
        //  sum: sum of segment
        u64 sum;
        //  num: length of segment
        u64 num;
        //  same: au64 i in [l, r) a_i == a
        bool same;
    };
    struct Act {
      private:
        //  a: divide
        //  b: update
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
