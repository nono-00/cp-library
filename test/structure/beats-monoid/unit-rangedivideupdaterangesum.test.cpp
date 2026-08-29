// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <random>
#include <vector>

#include "nono/ds/segment-tree-beats.hpp"
#include "nono/structure/beats-monoid/range-divide-update-range-sum.hpp"

namespace nono {

void solve() {
    using Monoid = beats_monoid::RangeDivideUpdateRangeSum;
    using Value = Monoid::Value;
    using Act = Monoid::Act;
    using u64 = std::uint64_t;

    std::mt19937 mt(555);
    std::uniform_int_distribution<u64> val(1, 1000);
    std::uniform_int_distribution<u64> divisor(1, 5);

    //  nはpower of 2ではないケースも含めて広くカバーする
    for (int n: {1, 2, 3, 5, 7, 8, 16, 17, 30}) {
        std::vector<u64> a(n);
        for (auto& x: a) x = val(mt);
        std::vector<Value> init(a.begin(), a.end());
        SegmentTreeBeats<Monoid> seg(init);

        std::uniform_int_distribution<int> pos(0, n - 1);
        for (int iter = 0; iter < 200; iter++) {
            int l = pos(mt), r = pos(mt);
            if (l > r) std::swap(l, r);
            r++;
            if (iter % 2 == 0) {
                u64 d = divisor(mt);
                seg.apply(l, r, Act::divide(d));
                for (int i = l; i < r; i++) a[i] /= d;
            } else {
                u64 v = val(mt);
                seg.apply(l, r, Act::update(v));
                for (int i = l; i < r; i++) a[i] = v;
            }

            int ql = pos(mt), qr = pos(mt);
            if (ql > qr) std::swap(ql, qr);
            qr++;
            u64 expected_sum = 0;
            for (int i = ql; i < qr; i++) expected_sum += a[i];
            assert(seg.prod(ql, qr).sum == expected_sum);
        }

        //  全区間の合計も別途確認
        u64 total = 0;
        for (auto x: a) total += x;
        assert(seg.prod(0, n).sum == total);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
