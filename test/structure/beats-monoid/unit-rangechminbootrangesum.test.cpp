// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <limits>
#include <random>
#include <vector>

#include "nono/ds/segment-tree-beats.hpp"
#include "nono/structure/beats-monoid/range-chmin-boot-range-sum.hpp"

namespace nono {

void solve() {
    using Monoid = beats_monoid::RangeChminBootRangeSum<long long>;
    using Value = Monoid::Value;
    using Act = Monoid::Act;

    //  Part A: boot()を使わず、chminのみを実配列との突き合わせでstress検証する
    //  （boot()はアグリゲート情報を意図的に欠落させる操作のため、
    //   「実配列1つ1つの値」というモデルでは検証できない。Part Bで直接検証する）
    std::mt19937 mt(4242);
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
            long long v = val(mt);
            seg.apply(l, r, Act::chmin(v));
            for (int i = l; i < r; i++) a[i] = std::min(a[i], v);

            int ql = pos(mt), qr = pos(mt);
            if (ql > qr) std::swap(ql, qr);
            qr++;
            long long expected_sum = 0;
            long long expected_max = std::numeric_limits<long long>::min();
            for (int i = ql; i < qr; i++) {
                expected_sum += a[i];
                expected_max = std::max(expected_max, a[i]);
            }
            Value got = seg.prod(ql, qr);
            assert(got.sum == expected_sum);
            assert(got.max_top2[0] == expected_max);
        }
    }

    //  Part B: boot()自体の仕様（mapping関数レベル）を直接検証する
    //  boot(): 「現在のmax_top2[0]の値のまま、num=1・sum=max_top2[0]に潰す」
    //  注意: boot()はsegment tree beatsが実際に触れたノード単位でしか収束しない
    //  （クエリ範囲全体で重複がグローバルに1つに潰れることは保証されない）ため、
    //  end-to-endのsegment tree経由テストではなくmapping()を直接呼んで検証する
    {
        //  複数要素がmax値を共有している状況を作る
        Monoid::Value v;
        v.max_top2 = {5, 3};
        v.num = 4;
        v.sum = 100;
        auto result = Monoid::mapping(Act::boot(), v);
        assert(result.has_value());
        assert(result->max_top2[0] == 5 && result->max_top2[1] == 3);
        assert(result->num == 1);
        assert(result->sum == 5);
    }
    {
        //  chminとbootを同時に適用するAct（複合actとして構築されるケース）
        Monoid::Value v;
        v.max_top2 = {10, 2};
        v.num = 3;
        v.sum = 40;
        Act chmin_and_boot{6, true};
        auto result = Monoid::mapping(chmin_and_boot, v);
        assert(result.has_value());
        //  chmin(6): max_top2[0]=10 -> 6 (6 > max_top2[1]=2 なので適用可能)
        //  sum -= (10-6)*num=3 -> 40-12=28, max_top2[0]=6
        //  boot: num=1, sum=max_top2[0]=6
        assert(result->max_top2[0] == 6 && result->max_top2[1] == 2);
        assert(result->num == 1);
        assert(result->sum == 6);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
