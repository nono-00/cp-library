#define PROBLEM "https://judge.yosupo.jp/problem/static_range_sum"
#include <iostream>
#include <vector>

#include "nono/ds/segment-tree.hpp"

namespace nono {

template <class T>
struct Add {
    using Value = T;
    static Value op(Value lhs, Value rhs) {
        return lhs + rhs;
    }
    static Value e() {
        return 0;
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    const SegmentTree<Add<long long>> segtree(a);
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
