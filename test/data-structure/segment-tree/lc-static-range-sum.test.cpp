#define PROBLEM "https://judge.yosupo.jp/problem/static_range_sum"
#include <iostream>
#include <vector>

#include "nono/data-structure/segment-tree.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    SumSegmentTree<long long> segtree(a);
    while (q--) {
        int l, r;
        std::cin >> l >> r;
        std::cout << segtree.prod(l, r) << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
