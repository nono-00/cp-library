// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <limits>
#include <random>
#include <vector>

#include "nono/ds/lazy-segment-tree.hpp"
#include "nono/structure/act-monoid/range-chmin-chmax-add-range-min-max.hpp"

namespace nono {

void solve() {
    using Monoid = act_monoid::RangeChminChmaxAddRangeMinMax<long long>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;

    std::mt19937 mt(2024);
    std::uniform_int_distribution<long long> val(-1000, 1000);

    for (int n : {1, 2, 3, 7, 16, 30}) {
        std::vector<long long> a(n);
        for (auto& x: a) x = val(mt);
        std::vector<Value> init(a.begin(), a.end());
        LazySegmentTree<Monoid> seg(init);

        std::uniform_int_distribution<int> pos(0, n - 1);
        for (int iter = 0; iter < 200; iter++) {
            int l = pos(mt), r = pos(mt);
            if (l > r) std::swap(l, r);
            r++;
            int op = iter % 4;
            if (op == 0) {
                long long v = val(mt);
                seg.apply(l, r, Act::chmin(v));
                for (int i = l; i < r; i++) a[i] = std::min(a[i], v);
            } else if (op == 1) {
                long long v = val(mt);
                seg.apply(l, r, Act::chmax(v));
                for (int i = l; i < r; i++) a[i] = std::max(a[i], v);
            } else if (op == 2) {
                long long v = val(mt);
                seg.apply(l, r, Act::add(v));
                for (int i = l; i < r; i++) a[i] += v;
            } else {
                long long lo = val(mt), hi = val(mt);
                if (lo > hi) std::swap(lo, hi);
                seg.apply(l, r, Act::clamp(lo, hi));
                for (int i = l; i < r; i++) a[i] = std::clamp(a[i], lo, hi);
            }

            //  ランダムな区間でmin/maxを突き合わせる
            int ql = pos(mt), qr = pos(mt);
            if (ql > qr) std::swap(ql, qr);
            qr++;
            long long expected_min = std::numeric_limits<long long>::max();
            long long expected_max = std::numeric_limits<long long>::min();
            for (int i = ql; i < qr; i++) {
                expected_min = std::min(expected_min, a[i]);
                expected_max = std::max(expected_max, a[i]);
            }
            Value got = seg.prod(ql, qr);
            assert(got.min == expected_min);
            assert(got.max == expected_max);
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
