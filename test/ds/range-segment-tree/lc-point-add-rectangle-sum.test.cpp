#define PROBLEM "https://judge.yosupo.jp/problem/point_add_rectangle_sum"
#include <array>
#include <iostream>
#include <vector>

#include "nono/ds/range-segment-tree.hpp"
#include "nono/structure/monoid.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<std::pair<int, int>> points;
    std::vector<long long> weight;
    points.reserve(n);
    weight.reserve(n);
    for (int i = 0; i < n; i++) {
        int x, y;
        long long w;
        std::cin >> x >> y >> w;
        points.emplace_back(x, y);
        weight.push_back(w);
    }
    using Query = std::array<int, 5>;
    std::vector<Query> querys(q);
    for (int i = 0; i < q; i++) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int x, y, w;
            std::cin >> x >> y >> w;
            points.emplace_back(x, y);
            weight.push_back(w);
            querys[i] = {t, x, y, w};
        } else {
            int l, d, r, u;
            std::cin >> l >> d >> r >> u;
            querys[i] = {t, l, d, r, u};
        }
    }

    RangeSegmentTree<monoid::Add<long long>, int> segtree(points);
    for (int i = 0; i < n; i++) {
        auto [x, y] = points[i];
        segtree.set(x, y, segtree.get(x, y) + weight[i]);
    }
    for (const auto& query: querys) {
        if (query[0] == 0) {
            int x = query[1];
            int y = query[2];
            long long w = query[3];
            segtree.set(x, y, segtree.get(x, y) + w);
        } else {
            int l = query[1];
            int d = query[2];
            int r = query[3];
            int u = query[4];
            std::cout << segtree.prod(l, d, r, u) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
