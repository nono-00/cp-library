#define PROBLEM "https://judge.yosupo.jp/problem/segment_add_get_min"

#include <iostream>

#include "nono/ds/dynamic-li-chao-tree.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    DynamicLiChaoTree<long long, int> tree;
    for (int i = 0; i < n; i++) {
        int l, r;
        long long a, b;
        std::cin >> l >> r >> a >> b;
        tree.add_segment(l, r, a, b);
    }
    for (int i = 0; i < q; i++) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r;
            long long a, b;
            std::cin >> l >> r >> a >> b;
            tree.add_segment(l, r, a, b);
        } else {
            int p;
            std::cin >> p;
            auto result = tree.get(p);
            if (!result) {
                std::cout << "INFINITY" << '\n';
            } else {
                std::cout << *result << '\n';
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
