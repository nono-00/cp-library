#define PROBLEM "https://judge.yosupo.jp/problem/rectangle_sum"
#include <iostream>
#include <vector>

#include "nono/data-structure/fenwick-range-tree.hpp"

namespace nono {

template <class T>
struct Add {
    using value_type = T;
    static T op(T lhs, T rhs) {
        return lhs + rhs;
    }
    static T e() {
        return T(0);
    }
    static T inv(T elem) {
        return -elem;
    }
};

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
    FenwickRangeTree<Add<long long>, int> fen(points);
    for (int i = 0; i < n; i++) {
        auto [x, y] = points[i];
        fen.apply(x, y, weight[i]);
    }
    while (q--) {
        int l, d, r, u;
        std::cin >> l >> d >> r >> u;
        std::cout << fen.prod(l, d, r, u) << '\n';
    }
}
}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
