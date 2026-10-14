#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace nono {

//  # SegmentTree
//  一点更新 区間取得のsegment tree. <https://atcoder.github.io/ac-library/master/document_ja/segtree.html>
//  Mはモノイド。添字が範囲外だと壊れる(pは [0, n)、l, rは 0 <= l <= r <= n)
template <class M>
class SegmentTree {
    using T = M::Value;

  public:
    SegmentTree(): SegmentTree(0) {}
    //  # SegmentTree(n)
    //  長さnで、全要素を M::e() にする
    //  O(n)
    explicit SegmentTree(int n): SegmentTree(std::vector<T>(n, M::e())) {}
    //  # SegmentTree(v)
    //  vの内容で初期化する
    //  O(n)
    explicit SegmentTree(const std::vector<T>& v): n_(int(v.size())) {
        size_ = std::bit_ceil((unsigned int)(n_));
        log_ = std::countr_zero((unsigned int)size_);
        data_ = std::vector<T>(2 * size_, M::e());
        for (int i = 0; i < n_; i++) data_[size_ + i] = v[i];
        for (int i = size_ - 1; i >= 1; i--) {
            update(i);
        }
    }

    //  # set(p, x)
    //  data[p] <= x
    //  O(log n)
    void set(int p, T x) {
        assert(0 <= p && p < n_);
        p += size_;
        data_[p] = x;
        for (int i = 1; i <= log_; i++) update(p >> i);
    }

    //  # get(p)
    //  return data[p]
    //  O(1)
    T get(int p) const {
        assert(0 <= p && p < n_);
        return data_[p + size_];
    }

    //  # prod(l, r)
    //  return op[for i in [l, r)](data[i])
    //  O(log n)
    T prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        T sml = M::e(), smr = M::e();
        l += size_;
        r += size_;

        while (l < r) {
            if (l & 1) sml = M::op(sml, data_[l++]);
            if (r & 1) smr = M::op(data_[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return M::op(sml, smr);
    }

    //  # all_prod()
    //  return op[for i in [0, n)](data[i])
    //  O(1)
    T all_prod() const {
        return data_[1];
    }

    //  # max_right(l, f)
    //  f(op[for i in [l, r)](data[i])) == trueを満たす最大のrを返す
    //  f(M::e()) がtrueでないと壊れる
    //  O(log n)
    template <class F>
    int max_right(int l, F f) const {
        assert(0 <= l && l <= n_);
        assert(f(M::e()));
        if (l == n_) return n_;
        l += size_;
        T sm = M::e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(M::op(sm, data_[l]))) {
                while (l < size_) {
                    l = (2 * l);
                    if (f(M::op(sm, data_[l]))) {
                        sm = M::op(sm, data_[l]);
                        l++;
                    }
                }
                return l - size_;
            }
            sm = M::op(sm, data_[l]);
            l++;
        } while ((l & -l) != l);
        return n_;
    }

    //  # min_left(r, f)
    //  f(op[for i in [l, r)](data[i])) == trueを満たす最小のlを返す
    //  f(M::e()) がtrueでないと壊れる
    //  O(log n)
    template <class F>
    int min_left(int r, F f) const {
        assert(0 <= r && r <= n_);
        assert(f(M::e()));
        if (r == 0) return 0;
        r += size_;
        T sm = M::e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(M::op(data_[r], sm))) {
                while (r < size_) {
                    r = (2 * r + 1);
                    if (f(M::op(data_[r], sm))) {
                        sm = M::op(data_[r], sm);
                        r--;
                    }
                }
                return r + 1 - size_;
            }
            sm = M::op(data_[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

  private:
    int n_, size_, log_;
    std::vector<T> data_;

    void update(int k) {
        data_[k] = M::op(data_[2 * k], data_[2 * k + 1]);
    }
};

}  //  namespace nono
