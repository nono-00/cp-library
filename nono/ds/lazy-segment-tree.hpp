#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace nono {

///  # 遅延セグ木
///  区間作用 区間取得のsegment tree. acl のインターフェースを少し変えただけ.
///  https://atcoder.github.io/ac-library/master/document_ja/lazysegtree.html
template <class M>
struct LazySegmentTree {
    using T = M::Value;
    using F = M::Act;

  public:
    LazySegmentTree(): LazySegmentTree(0) {}
    explicit LazySegmentTree(int n): LazySegmentTree(std::vector<T>(n, M::e())) {}
    explicit LazySegmentTree(const std::vector<T>& v): n_(int(v.size())) {
        size_ = std::bit_ceil((unsigned int)(n_));
        log_ = std::countr_zero((unsigned int)size_);
        data_ = std::vector<T>(2 * size_, M::e());
        lazy_ = std::vector<F>(size_, M::id());
        for (int i = 0; i < n_; i++) data_[size_ + i] = v[i];
        for (int i = size_ - 1; i >= 1; i--) {
            update(i);
        }
    }

    ///  # set(p, x)
    ///  data[p] <= x
    ///  O(logn)
    void set(int p, T x) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; i--) push(p >> i);
        data_[p] = x;
        for (int i = 1; i <= log_; i++) update(p >> i);
    }

    ///  # get(p)
    ///  return data[p]
    ///  O(logn)
    T get(int p) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; i--) push(p >> i);
        return data_[p];
    }

    ///  # prod(l, r)
    ///  return op[for i in [l, r)](data[i])
    ///  O(logn)
    T prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return M::e();

        l += size_;
        r += size_;

        for (int i = log_; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        T sml = M::e(), smr = M::e();
        while (l < r) {
            if (l & 1) sml = M::op(sml, data_[l++]);
            if (r & 1) smr = M::op(data_[--r], smr);
            l >>= 1;
            r >>= 1;
        }

        return M::op(sml, smr);
    }

    ///  # all_prod()
    ///  O(1)
    T all_prod() {
        return data_[1];
    }

    ///  # apply(p, f)
    ///  data[p] <= mapping(f, data[p])
    ///  O(logn)
    void apply(int p, F f) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; i--) push(p >> i);
        data_[p] = M::mapping(f, data_[p]);
        for (int i = 1; i <= log_; i++) update(p >> i);
    }
    ///  # apply(l, r, f)
    ///  [for i in [l, r)](data[i] <= mapping(f, data[i])
    ///  O(logn)
    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return;

        l += size_;
        r += size_;

        for (int i = log_; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        {
            int l2 = l, r2 = r;
            while (l < r) {
                if (l & 1) all_apply(l++, f);
                if (r & 1) all_apply(--r, f);
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;
        }

        for (int i = 1; i <= log_; i++) {
            if (((l >> i) << i) != l) update(l >> i);
            if (((r >> i) << i) != r) update((r - 1) >> i);
        }
    }

    ///  # max_right(l, f)
    ///  return max { r | r in (l, n], G(prod(l, r)) is true}
    ///  O(logn)
    template <class G>
    int max_right(int l, G g) {
        assert(0 <= l && l <= n_);
        assert(g(M::e()));
        if (l == n_) return n_;
        l += size_;
        for (int i = log_; i >= 1; i--) push(l >> i);
        T sm = M::e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!g(M::op(sm, data_[l]))) {
                while (l < size_) {
                    push(l);
                    l = (2 * l);
                    if (g(M::op(sm, data_[l]))) {
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

    ///  # min_left(r, f)
    ///  return min { l | l in [0, r), G(prod(l, r)) is true}
    ///  O(logn)
    template <class G>
    int min_left(int r, G g) {
        assert(0 <= r && r <= n_);
        assert(g(M::e()));
        if (r == 0) return 0;
        r += size_;
        for (int i = log_; i >= 1; i--) push((r - 1) >> i);
        T sm = M::e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!g(M::op(data_[r], sm))) {
                while (r < size_) {
                    push(r);
                    r = (2 * r + 1);
                    if (g(M::op(data_[r], sm))) {
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
    std::vector<F> lazy_;

    void update(int k) {
        data_[k] = M::op(data_[2 * k], data_[2 * k + 1]);
    }
    void all_apply(int k, F f) {
        data_[k] = M::mapping(f, data_[k]);
        if (k < size_) lazy_[k] = M::composition(f, lazy_[k]);
    }
    void push(int k) {
        all_apply(2 * k, lazy_[k]);
        all_apply(2 * k + 1, lazy_[k]);
        lazy_[k] = M::id();
    }
};

}  //  namespace nono
