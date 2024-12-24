#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/5/DSL_5_B"
#include <algorithm>
#include <iostream>

#include "../../../nono/ds/dynamic-segment-tree-2d.hpp"
#include "../../../nono/structure/monoid.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    using Monoid = monoid::Add<int>;
    DynamicSegmentTree2D<Monoid> segtree(0, 1e5, 0, 1e5);
    auto put = [&](int x1, int y1, int diff) {
        auto val = segtree.prod(x1, y1, x1 + 1, y1 + 1);
        segtree.set(x1, y1, val + diff);
    };
    for (int i = 0; i < n; i++) {
        int x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;

        put(x1, y1, 1);
        put(x1, y2, -1);
        put(x2, y1, -1);
        put(x2, y2, 1);
    }

    int ans = 0;
    for (int i = 0; i <= 1000; i++) {
        for (int j = 0; j <= 1000; j++) {
            ans = std::max(ans, segtree.prod(0, 0, i, j));
        }
    }
    std::cout << ans << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
