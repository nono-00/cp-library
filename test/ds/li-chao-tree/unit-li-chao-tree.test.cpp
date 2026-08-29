// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/ds/li-chao-tree.hpp"

namespace nono {

void solve() {
    struct Seg {
        long long a, b;
        int l, r;
    };

    std::mt19937 mt(42);
    constexpr int N = 200;
    std::uniform_int_distribution<long long> coef(-100, 100);
    std::uniform_int_distribution<int> pos(0, N - 1);

    MinLiChaoTree<long long> min_tree(N);
    MaxLiChaoTree<long long> max_tree(N);
    std::vector<Seg> segs;

    for (int iter = 0; iter < 50; iter++) {
        long long a = coef(mt), b = coef(mt);
        int l = pos(mt), r = pos(mt);
        if (l > r) std::swap(l, r);
        if (l == r) r++;
        if (r > N) r = N;
        if (l >= r) continue;

        min_tree.add_segment(l, r, a, b);
        max_tree.add_segment(l, r, a, b);
        segs.push_back({a, b, l, r});

        for (int x = 0; x < N; x++) {
            bool any = false;
            long long mn = 0, mx = 0;
            for (auto& s: segs) {
                if (s.l <= x && x < s.r) {
                    long long v = s.a * x + s.b;
                    if (!any || v < mn) mn = v;
                    if (!any || v > mx) mx = v;
                    any = true;
                }
            }
            auto got_min = min_tree.get(x);
            auto got_max = max_tree.get(x);
            if (any) {
                assert(got_min && *got_min == mn);
                assert(got_max && *got_max == mx);
            } else {
                assert(!got_min);
                assert(!got_max);
            }
        }
    }

    //  add_line: [0, N)全体をカバーする
    MinLiChaoTree<long long> full(N);
    full.add_line(2, 3);
    for (int x = 0; x < N; x++) {
        auto v = full.get(x);
        assert(v && *v == 2 * x + 3);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
