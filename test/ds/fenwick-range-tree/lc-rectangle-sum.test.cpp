#define PROBLEM "https://judge.yosupo.jp/problem/rectangle_sum"
#include <iostream>
#include <vector>

#include "nono/ds/fenwick-range-tree.hpp"

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
    FenwickRangeTree<long long, int> fen(points);
    for (int i = 0; i < n; i++) {
        auto [x, y] = points[i];
        fen.add(x, y, weight[i]);
    }
    while (q--) {
        int l, d, r, u;
        std::cin >> l >> d >> r >> u;
        std::cout << fen.sum(l, d, r, u) << '\n';
    }
}
}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
