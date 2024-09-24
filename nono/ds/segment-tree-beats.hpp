#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace nono {

///  brief : 時々作用がミスるlazy segment tree. だいたいこれ. <https://rsm9.hatenablog.com/entry/2021/02/01/220408>
template <class M>
struct SegmentTreeBeats {
    using T = M::Value;
    using F = M::Act;

  public:
    SegmentTreeBeats(): SegmentTreeBeats(0) {}
    explicit SegmentTreeBeats(int n): SegmentTreeBeats(std::vector<T>(n, M::e())) {}
    explicit SegmentTreeBeats(const std::vector<T>& v): n_(int(v.size())) {
        size_ = std::bit_ceil((unsigned int)(n_));
        log_ = std::countr_zero((unsigned int)size_);
        data_ = std::vector<T>(2 * size_, M::e());
        lazy_ = std::vector<F>(size_, M::id());
        for (int i = 0; i < n_; i++) data_[size_ + i] = v[i];
        for (int i = size_ - 1; i >= 1; i--) {
            update(i);
        }
    }

    void set(int p, T x) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; i--) push(p >> i);
        data_[p] = x;
        for (int i = 1; i <= log_; i++) update(p >> i);
    }

    T get(int p) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; i--) push(p >> i);
        return data_[p];
    }

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

    T all_prod() {
        return data_[1];
    }

    void apply(int p, F f) {
        assert(0 <= p && p < n_);
        p += size_;
        for (int i = log_; i >= 1; i--) push(p >> i);
        data_[p] = M::mapping(f, data_[p]);
        for (int i = 1; i <= log_; i++) update(p >> i);
    }
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
        auto result = M::mapping(f, data_[k]);
        if (k < size_) {
            lazy_[k] = M::composition(f, lazy_[k]);
            if (!result) {
                push(k);
                update(k);
                return;
            }
        }
        data_[k] = result.value();
    }
    void push(int k) {
        all_apply(2 * k, lazy_[k]);
        all_apply(2 * k + 1, lazy_[k]);
        lazy_[k] = M::id();
    }
};

}  //  namespace nono

