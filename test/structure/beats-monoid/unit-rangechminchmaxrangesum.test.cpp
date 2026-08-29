// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/ds/segment-tree-beats.hpp"
#include "nono/structure/beats-monoid/range-chmin-chmax-range-sum.hpp"

namespace nono {

void solve() {
    using Monoid = beats_monoid::RangeChminChmaxRangeSum<long long>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;

    std::mt19937 mt(999);
    std::uniform_int_distribution<long long> val(-1000, 1000);

    for (int n: {1, 2, 3, 7, 16, 30}) {
        std::vector<long long> a(n);
        for (auto& x: a) x = val(mt);
        std::vector<Value> init(a.begin(), a.end());
        SegmentTreeBeats<Monoid> seg(init);

        std::uniform_int_distribution<int> pos(0, n - 1);
        for (int iter = 0; iter < 300; iter++) {
            int l = pos(mt), r = pos(mt);
            if (l > r) std::swap(l, r);
            r++;
            if (iter % 2 == 0) {
                long long v = val(mt);
                seg.apply(l, r, Act::chmin(v));
                for (int i = l; i < r; i++) a[i] = std::min(a[i], v);
            } else {
                long long v = val(mt);
                seg.apply(l, r, Act::chmax(v));
                for (int i = l; i < r; i++) a[i] = std::max(a[i], v);
            }

            int ql = pos(mt), qr = pos(mt);
            if (ql > qr) std::swap(ql, qr);
            qr++;
            long long expected_sum = 0;
            for (int i = ql; i < qr; i++) expected_sum += a[i];
            assert(seg.prod(ql, qr).sum == expected_sum);
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
