#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

//  # RangeChminChmaxAddRangeSum
//  区間chmin・chmax・加算、区間和
//  作用は Act::chmin(val), Act::chmax(val), Act::add(val) で作る。1要素は Value(val) で作る
//  mappingは、区間の集約値だけでは作用後の和を決められないとき nullopt を返す(子に降りて計算し直す)
template <class T, class Count = unsigned>
struct RangeChminChmaxAddRangeSum {
  private:
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();

  public:
    struct Act {
      private:
        T chmin_, chmax_, add_;
        friend struct RangeChminChmaxAddRangeSum;

      public:
        Act(T a, T b, T c): chmin_(a), chmax_(b), add_(c) {}
        static Act chmin(T val) {
            return Act{val, MIN, 0};
        }
        static Act chmax(T val) {
            return Act{MAX, val, 0};
        }
        static Act add(T val) {
            return Act{MAX, MIN, val};
        }
    };
    struct Value {
        Value(): max_top2({MIN, MIN}), min_top2({MAX, MAX}), num_max(0), num_min(0), num(0), sum(0) {}
        Value(T val, Count num = 1)
            : max_top2({val, MIN}),
              min_top2({val, MAX}),
              num_max(num),
              num_min(num),
              num(num),
              sum(val) {}
        //  [0]: 最大値, [1]: 2番目に大きい値
        std::array<T, 2> max_top2;
        //  [0]: 最小値, [1]: 2番目に小さい値
        std::array<T, 2> min_top2;
        //  最大値の個数
        int num_max;
        //  最小値の個数
        int num_min;
        //  区間の長さ
        int num;
        //  区間の和
        T sum;
    };
    static Value op(Value lhs, Value rhs) {
        Value a;
        a.sum = lhs.sum + rhs.sum;
        a.num = lhs.num + rhs.num;
        {  //  min
            int l = 0, r = 0;
            for (int i = 0; i < 2; i++) {
                if (lhs.max_top2[l] == rhs.max_top2[r]) {
                    a.max_top2[i] = lhs.max_top2[l];
                    if (i == 0) a.num_max = lhs.num_max + rhs.num_max;
                    l++, r++;
                } else if (lhs.max_top2[l] < rhs.max_top2[r]) {
                    a.max_top2[i] = rhs.max_top2[r];
                    if (i == 0) a.num_max = rhs.num_max;
                    r++;
                } else {
                    a.max_top2[i] = lhs.max_top2[l];
                    if (i == 0) a.num_max = lhs.num_max;
                    l++;
                }
            }
        }
        {  //  max
            int l = 0, r = 0;
            for (int i = 0; i < 2; i++) {
                if (lhs.min_top2[l] == rhs.min_top2[r]) {
                    a.min_top2[i] = lhs.min_top2[l];
                    if (i == 0) a.num_min = lhs.num_min + rhs.num_min;
                    l++, r++;
                } else if (lhs.min_top2[l] > rhs.min_top2[r]) {
                    a.min_top2[i] = rhs.min_top2[r];
                    if (i == 0) a.num_min = rhs.num_min;
                    r++;
                } else {
                    a.min_top2[i] = lhs.min_top2[l];
                    if (i == 0) a.num_min = lhs.num_min;
                    l++;
                }
            }
        }
        return a;
    }
    static Value e() {
        return Value{};
    }
    static std::optional<Value> mapping(Act act, Value value) {
        if (act.chmin_ <= value.max_top2[1]) {
            return std::nullopt;
        }
        if (value.max_top2[0] > act.chmin_ && act.chmin_ > value.max_top2[1]) {
            value.sum -= (value.max_top2[0] - act.chmin_) * value.num_max;
            if (value.max_top2[0] == value.min_top2[1]) {
                value.min_top2[1] = act.chmin_;
            } else if (value.max_top2[0] == value.min_top2[0]) {
                value.min_top2[0] = act.chmin_;
            }
            value.max_top2[0] = act.chmin_;
        }
        if (act.chmax_ >= value.min_top2[1]) {
            return std::nullopt;
        }
        if (value.min_top2[0] < act.chmax_ && act.chmax_ < value.min_top2[1]) {
            value.sum += (act.chmax_ - value.min_top2[0]) * value.num_min;
            if (value.min_top2[0] == value.max_top2[1]) {
                value.max_top2[1] = act.chmax_;
            } else if (value.max_top2[0] == value.min_top2[0]) {
                value.max_top2[0] = act.chmax_;
            }
            value.min_top2[0] = act.chmax_;
        }
        value.sum += act.add_ * value.num;
        for (int i = 0; i < 2; i++) {
            if (value.max_top2[i] != MIN) value.max_top2[i] += act.add_;
            if (value.min_top2[i] != MAX) value.min_top2[i] += act.add_;
        }
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        T chmin = (lhs.chmin_ != MAX ? lhs.chmin_ - rhs.add_ : lhs.chmin_);
        T chmax = (lhs.chmax_ != MIN ? lhs.chmax_ - rhs.add_ : lhs.chmax_);
        if (rhs.chmin_ <= chmax) return Act{chmax, chmax, lhs.add_ + rhs.add_};
        if (chmin <= rhs.chmax_) return Act{chmin, chmin, lhs.add_ + rhs.add_};
        return Act{std::min(rhs.chmin_, chmin), std::max(rhs.chmax_, chmax), lhs.add_ + rhs.add_};
    }
    static Act id() {
        return Act{MAX, MIN, 0};
    }
};

}  //  namespace beats_monoid

}  //  namespace nono
