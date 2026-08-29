#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <optional>

namespace nono {

namespace beats_monoid {

///  # RangeChminChmaxRangeSum
template <class T, class Count = unsigned>
struct RangeChminChmaxRangeSum {
  private:
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();

  public:
    struct Value {
        Value(): max_top2({MIN, MIN}), min_top2({MAX, MAX}), num_max(0), num_min(0), sum(0) {}
        Value(T val, Count num = 1)
            : max_top2({val, MIN}),
              min_top2({val, MAX}),
              num_max(num),
              num_min(num),
              sum(val) {}
        //  a[0]: maximum value, v[1]: second maximum value
        std::array<T, 2> max_top2;
        //  b[0]: minimum value, v[1]: second minimum value
        std::array<T, 2> min_top2;
        //  n: num of maximum value
        Count num_max;
        //  m: num of minimum value
        Count num_min;
        //  s: sum of segment
        T sum;
    };
    struct Act {
      private:
        //  a: chmin
        //  b: chmax
        T chmin_, chmax_;
        friend struct RangeChminChmaxRangeSum;

      public:
        Act(T a, T b): chmin_(a), chmax_(b) {}
        static Act chmin(T val) {
            return Act{val, MIN};
        }
        static Act chmax(T val) {
            return Act{MAX, val};
        }
    };
    static Value op(Value lhs, Value rhs) {
        Value result;
        result.sum = lhs.sum + rhs.sum;
        {  //  min
            int l = 0, r = 0;
            for (int i = 0; i < 2; i++) {
                if (lhs.max_top2[l] == rhs.max_top2[r]) {
                    result.max_top2[i] = lhs.max_top2[l];
                    if (i == 0) result.num_max = lhs.num_max + rhs.num_max;
                    l++, r++;
                } else if (lhs.max_top2[l] < rhs.max_top2[r]) {
                    result.max_top2[i] = rhs.max_top2[r];
                    if (i == 0) result.num_max = rhs.num_max;
                    r++;
                } else {
                    result.max_top2[i] = lhs.max_top2[l];
                    if (i == 0) result.num_max = lhs.num_max;
                    l++;
                }
            }
        }
        {  //  max
            int l = 0, r = 0;
            for (int i = 0; i < 2; i++) {
                if (lhs.min_top2[l] == rhs.min_top2[r]) {
                    result.min_top2[i] = lhs.min_top2[l];
                    if (i == 0) result.num_min = lhs.num_min + rhs.num_min;
                    l++, r++;
                } else if (lhs.min_top2[l] > rhs.min_top2[r]) {
                    result.min_top2[i] = rhs.min_top2[r];
                    if (i == 0) result.num_min = rhs.num_min;
                    r++;
                } else {
                    result.min_top2[i] = lhs.min_top2[l];
                    if (i == 0) result.num_min = lhs.num_min;
                    l++;
                }
            }
        }
        return result;
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
        return value;
    }
    static Act composition(Act lhs, Act rhs) {
        if (lhs.chmin_ <= rhs.chmax_) return Act{lhs.chmin_, lhs.chmin_};
        if (rhs.chmin_ <= lhs.chmax_) return Act{lhs.chmax_, lhs.chmax_};
        return Act{std::min(lhs.chmin_, rhs.chmin_), std::max(lhs.chmax_, rhs.chmax_)};
    }
    static Act id() {
        return Act{MAX, MIN};
    }
};

}  //  namespace beats_monoid

}  //  namespace nono
