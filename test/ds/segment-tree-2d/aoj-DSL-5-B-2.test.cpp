#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/5/DSL_5_B"
#include <iostream>
#include <tuple>
#include <vector>

#include "../../../nono/ds/segment-tree-2d.hpp"
#include "../../../nono/structure/monoid.hpp"
#include "../../../nono/utility/compressor.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<std::tuple<int, int, int, int>> p;
    std::vector<int> xs, ys;
    for (int i = 0; i < n; i++) {
        int x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;
        p.emplace_back(x1, y1, x2, y2);
        xs.push_back(x1);
        xs.push_back(x2);
        ys.push_back(y1);
        ys.push_back(y2);
    }
    Compressor x(xs);
    Compressor y(ys);
    int h = x.size();
    int w = y.size();
    SegmentTree2D<monoid::Add<int>> segtree(h, w);

    auto put = [&](int x1, int y1, int diff) {
        x1 = x.compress(x1);
        y1 = y.compress(y1);
        auto val = segtree.prod(x1, y1, x1 + 1, y1 + 1);
        segtree.set(x1, y1, val + diff);
    };

    for (auto [x1, y1, x2, y2]: p) {
        put(x1, y1, 1);
        put(x1, y2, -1);
        put(x2, y1, -1);
        put(x2, y2, 1);
    }

    int ans = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
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
