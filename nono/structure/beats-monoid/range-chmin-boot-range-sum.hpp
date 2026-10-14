#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

//  # RangeChminBootRangeSum
//  区間chmin、区間和。要素ごとに、和へ含めるかどうかを後から有効にできる
//  https://codeforces.com/contest/855/problem/F
//  Value(val, 0) で作った要素は和に含まれず、Act::boot() を作用させると含まれるようになる
//  作用は Act::chmin(val), Act::boot() で作る
//  boot()を複数要素の区間に作用させると壊れる(1点にだけ作用させること)
//  mappingは、actが2番目に大きい値以下のとき nullopt を返す(子に降りて計算し直す)
template <class T>
struct RangeChminBootRangeSum {
  private:
    static constexpr T MIN = std::numeric_limits<T>::min();
    static constexpr T MAX = std::numeric_limits<T>::max();

  public:
    struct Value {
        Value(): max_top2({MIN, MIN}), num(0), sum(0) {}
        Value(T val, int num = 1): max_top2({val, MIN}), num(num), sum(val * num) {}
        //  [0]: 最大値, [1]: 2番目に大きい値
        std::array<T, 2> max_top2;
        //  最大値の個数
        int num;
        //  区間の和
        T sum;
    };
    struct Act {
      private:
        T chmin_;
        bool boot_;
        friend struct RangeChminBootRangeSum;

      public:
        Act(T a, bool b): chmin_(a), boot_(b) {}
        static Act chmin(T val) {
            return Act{val, false};
        }
        static Act boot() {
            return Act{MAX, true};
        }
    };
    static Value op(Value lhs, Value rhs) {
        Value result;
        int l = 0, r = 0;
        result.sum = lhs.sum + rhs.sum;
        for (int i = 0; i < 2; i++) {
            if (lhs.max_top2[l] == rhs.max_top2[r]) {
                result.max_top2[i] = lhs.max_top2[l];
                if (i == 0) result.num = lhs.num + rhs.num;
                l++, r++;
            } else if (lhs.max_top2[l] < rhs.max_top2[r]) {
                result.max_top2[i] = rhs.max_top2[r];
                if (i == 0) result.num = rhs.num;
                r++;
            } else {
                result.max_top2[i] = lhs.max_top2[l];
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
        if (act.chmin_ >= value.max_top2[0]) {
            if (act.boot_) {
                value.num = 1;
                value.sum = value.max_top2[0];
            }
            return value;
        } else if (act.chmin_ > value.max_top2[1]) {
            value.sum -= (value.max_top2[0] - act.chmin_) * value.num;
            value.max_top2[0] = act.chmin_;
            if (act.boot_) {
                value.num = 1;
                value.sum = value.max_top2[0];
            }
            return value;
        } else {
            assert(!act.boot_);
            return std::nullopt;
        }
    }
    static Act composition(Act lhs, Act rhs) {
        return Act{std::min(lhs.chmin_, rhs.chmin_), lhs.boot_ || rhs.boot_};
    }
    static Act id() {
        return Act{MAX, false};
    }
};

}  //  namespace beats_monoid

}  //  namespace nono
