#define PROBLEM "https://judge.yosupo.jp/problem/point_add_range_sum"
#include <iostream>
#include <vector>

#include "data-structure/segment-tree.hpp"

int main() {
    using namespace nono;
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];

    SumSegmentTree<long long> segtree(a);

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p;
            long long x;
            std::cin >> p >> x;
            segtree.set(p, segtree.get(p) + x);
        } else {
            int l, r;
            std::cin >> l >> r;
            std::cout << segtree.prod(l, r) << std::endl;
        }
    }
}
