#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>

///  brief : segment-tree-beats用Monoid全部盛り.
namespace nono {
namespace beats_monoid {
// not verified
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
// not verified
template <class T, class Count = unsigned>
struct RangeChmaxRangeSum {
    struct Value {
        Value(): top2({std::numeric_limits<T>::max(), std::numeric_limits<T>::max()}), num(0), sum(0) {}
        Value(T val, Count num = 1): top2({val, std::numeric_limits<T>::max()}), num(num), sum(val) {}
        //  v[0]: minimum value, v[1]: second minimum value
        std::array<T, 2> top2;
        //  n: num of minimum value
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
// not verified
template <class T, class Count = unsigned>
struct RangeChminChmaxRangeSum {
  private:
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();

  public:
    struct Value {
        Value(): max_top2({MIN, MIN}), min_top2({MAX, MAX}), num_max(0), num_min(0), sum(0) {}
        Value(T val = 0, Count num = 1)
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
        //  success or fail to apply
        bool fail;
    };
    struct Act {
        //  a: chmin
        //  b: chmax
        T chmin_, chmax_;
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
        if (act.chmin_ <= value.max_top2[1] || act.chmax_ >= value.min_top2[1]) {
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
            return value;
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
template <class T, class Count = unsigned>
struct RangeChminChmaxAddRangeSum {
  private:
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();

  public:
    struct Act {
        T chmin_, chmax_, add_;
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
        //  a[0]: maximum value, v[1]: second maximum value
        std::array<T, 2> max_top2;
        //  b[0]: minimum value, v[1]: second minimum value
        std::array<T, 2> min_top2;
        //  n: num of maximum value
        int num_max;
        //  m: num of minimum value
        int num_min;
        //  l: num of value
        int num;
        //  s: sum of segment
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
//  https://atcoder.jp/contests/abc256/tasks/abc256_h
struct RangeDivideUpdateRangeSum {
  private:
    using u64 = std::uint64_t;
    static constexpr u64 divide_id = 1;
    static constexpr u64 update_id = -1;

  public:
    struct Value {
        Value(): left(0), sum(0), num(1), same(true) {}
        Value(u64 v): left(v), sum(v), num(1), same(true) {}
        //  left: most left value
        u64 left;
        //  sum: sum of segment
        u64 sum;
        //  num: length of segment
        u64 num;
        //  same: au64 i in [l, r) a_i == a
        bool same;
        bool fail;
    };
    struct Act {
        //  a: divide
        //  b: update
        u64 divide_;
        std::optional<u64> update_;
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
//  https://codeforces.com/contest/855/problem/F
//  not verified
template <class T>
struct RangeChminBootRangeSum {
  private:
    static constexpr T MIN = std::numeric_limits<T>::min();
    static constexpr T MAX = std::numeric_limits<T>::max();

  public:
    struct Value {
        Value(): max_top2({MAX, MIN}), num(0), sum(0) {}
        //  v[0]: maximum value, v[1]: second maximum value
        std::array<T, 2> max_top2;
        //  n: num of maximum value
        int num;
        //  s: sum of segment
        T sum;
        //  success or fail to apply
        bool fail;
    };
    struct Act {
        T chmin_;
        bool boot_;
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
