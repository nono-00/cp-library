#pragma once

#include <algorithm>
#include <limits>

namespace nono {

namespace act_monoid {

//  # RangeChminChmaxAddRangeMinMax
//  区間chmin・chmax・加算、区間の最小値と最大値
//  Act (low, high, add) (x) -> min(high, max(low, x + add))
//  作用は Act::chmin(a), Act::chmax(a), Act::clamp(low, high), Act::add(b) で作る
//  clamp(low, high) は low <= high でないと壊れる
//  Tが浮動小数点数だと壊れる(numeric_limits<T>::min() が正の最小値になるため)
template <class T>
struct RangeChminChmaxAddRangeMinMax {
  private:
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();

  public:
    struct Value {
        Value(): min(MAX), max(MIN) {}
        Value(T value): min(value), max(value) {}
        Value(T min, T max): min(min), max(max) {}
        T min;
        T max;
    };
    struct Act {
      private:
        T chmin_;
        T chmax_;
        T add_;
        friend struct RangeChminChmaxAddRangeMinMax;

      public:
        static Act chmin(T a) {
            return Act{a, MIN, static_cast<T>(0)};
        }
        static Act chmax(T a) {
            return Act{MAX, a, static_cast<T>(0)};
        }
        static Act clamp(T low, T high) {
            return Act{high, low, static_cast<T>(0)};
        }
        static Act add(T b) {
            return Act{MAX, MIN, b};
        }
        Act(T a, T b, T c): chmin_(a), chmax_(b), add_(c) {}
    };
    static Value op(Value lhs, Value rhs) {
        return Value{std::min(lhs.min, rhs.min), std::max(lhs.max, rhs.max)};
    }
    static Value e() {
        return Value{};
    }
    static Value mapping(Act act, Value value) {
        return Value{
            std::clamp(value.min + act.add_, act.chmax_, act.chmin_),
            std::clamp(value.max + act.add_, act.chmax_, act.chmin_),
        };
    }
    //  mapping(act, x) = clamp(x + act.add_, act.chmax_, act.chmin_) であり、
    //  clampがadd_の後に効くため、rhsのchmin_/chmax_をlhs.add_だけ平行移動して
    //  lhs側の境界と比較する（lhsのadd_ではなくrhsのadd_を引く旧実装は誤り）
    static Act composition(Act lhs, Act rhs) {
        T chmin = (rhs.chmin_ != MAX ? rhs.chmin_ + lhs.add_ : rhs.chmin_);
        T chmax = (rhs.chmax_ != MIN ? rhs.chmax_ + lhs.add_ : rhs.chmax_);
        if (chmin <= lhs.chmax_) return Act{lhs.chmax_, lhs.chmax_, lhs.add_ + rhs.add_};
        if (lhs.chmin_ <= chmax) return Act{lhs.chmin_, lhs.chmin_, lhs.add_ + rhs.add_};
        return Act{std::min(lhs.chmin_, chmin), std::max(lhs.chmax_, chmax), lhs.add_ + rhs.add_};
    }
    static Act id() {
        return Act{MAX, MIN, static_cast<T>(0)};
    }
};

}  //  namespace act_monoid

}  //  namespace nono
