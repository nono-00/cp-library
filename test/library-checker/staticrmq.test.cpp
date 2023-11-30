#define PROBLEM "https://judge.yosupo.jp/problem/staticrmq"
#include <iostream>
#include <vector>

#include "data-structure/segment-tree.hpp"

int main() {
    using namespace nono;
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];

    MinSegmentTree<long long> segtree(a);

    while (q--) {
        int l, r;
        std::cin >> l >> r;
        std::cout << segtree.prod(l, r) << std::endl;
    }
}
