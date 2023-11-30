#define PROBLEM "https://judge.yosupo.jp/problem/static_range_sum"
#include <iostream>
#include <vector>

#include "data-structure/fenwick-tree.hpp"

int main() {
    std::cin.tie(0)->sync_with_stdio(0);

    using namespace nono;
    int n, q;
    std::cin >> n >> q;
    FenwickTree<long long> fenwick(n);
    for (int i = 0; i < n; i++) {
        long long v;
        std::cin >> v;
        fenwick.add(i, v);
    }

    while (q--) {
        int l, r;
        std::cin >> l >> r;
        std::cout << fenwick.sum(l, r) << '\n';
    }
}
