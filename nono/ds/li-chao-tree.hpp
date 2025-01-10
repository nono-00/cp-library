#pragma once

#include <bit>
#include <cassert>
#include <numeric>
#include <optional>
#include <vector>

namespace nono {

///  # LiChao木
///  直線, 線分追加, ある地点での最小値/最大値取得ができる
///  MinLiChaoTree, MaxLiChaoTreeを定義してあるので使うと良い
template <class T, bool minimum = true>
class LiChaoTree {
    static constexpr T compare = minimum ? 1 : -1;
    struct Line {
        Line(T a = 0, T b = 0): a(a), b(b) {}
        T a, b;
        T eval(int x) {
            return a * x + b;
        }
    };

  public:
    LiChaoTree(int n): n_(std::bit_ceil((unsigned)n_)), data_(2 * n) {}
    ///  # add_line(a, b)
    ///  [0, n)にax + bを追加する
    ///  O(logn)
    void add_line(T a, T b) {
        add_segment(0, n_, a, b);
    }
    ///  # add_segment
    ///  [l, r)にax+bを追加
    ///  O(log n)
    void add_segment(int l, int r, T a, T b) {
        assert(0 <= l && l < r && r <= n_);
        add(0, n_, l, r, 1, Line(compare * a, compare * b));
    }
    ///  # get(x)
    ///  min/max[forall (a, b) in {Line or Segment}](ax + b)
    ///  線分、直線が存在しない場合、nullopt
    std::optional<T> get(int i) {
        assert(0 <= i && i < n_);
        auto result = get(0, n_, i, 1);
        return result ? compare * *result : result;
    }

  private:
    void add(int l, int r, int L, int R, int i, Line v) {
        assert(0 <= l && l < r && r <= n_);
        if (r <= L || R <= l) return;
        if (L <= l && r <= R) {
            update(i, l, r, v);
        } else {
            int m = (l + r) / 2;
            add(l, m, L, R, 2 * i, v);
            add(m, r, L, R, 2 * i + 1, v);
        }
    }

    void update(int i, int l, int r, Line v) {
        assert(0 <= l && l < r && r <= n_);
        if (!data_[i]) {
            data_[i] = v;
            return;
        }
        int m = (l + r) / 2;
        bool lhs = data_[i].eval(l) < v.eval(l);
        bool mhs = data_[i].eval(m) < v.eval(m);
        bool rhs = data_[i].eval(r - 1) < v.eval(r - 1);
        if (lhs && mhs && rhs) return;
        if (!lhs && !mhs && !rhs) {
            data_[i] = v;
            return;
        }
        if (lhs ^ mhs ^ rhs) {
            swap(v, data_[i]);
            lhs ^= true, mhs ^= true, rhs ^= true;
        }
        if (lhs) {
            update(2 * i + 1, m, r, v);
        } else {
            update(2 * i, l, m, v);
        }
    }
    std::optional<T> get(int l, int r, int I, int i) {
        assert(0 <= l && l < r && r <= n_);
        std::optional<T> v = data_[i] ? data_[i].eval(I) : std::nullopt;
        if (l == I && l + 1 == r) {
            return v;
        }
        int m = std::midpoint(l, r);
        if (l <= I && I < m) {
            auto result = get(l, m, I, 2 * i);
            if (v && result) {
                v = std::min(v, result);
            } else if (result) {
                v = result;
            }
        } else {
            auto result = get(m, r, I, 2 * i + 1);
            if (v && result) {
                v = std::min(v, result);
            } else if (result) {
                v = result;
            }
        }
        return v;
    }

    int n_;
    std::vector<std::optional<Line>> data_;
};

template <class T>
using MinLiChaoTree = LiChaoTree<T, true>;

template <class T>
using MaxLiChaoTree = LiChaoTree<T, false>;

}  //  namespace nono