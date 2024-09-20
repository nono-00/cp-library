#define PROBLEM "https://judge.yosupo.jp/problem/line_add_get_min"

#include <iostream>

#include "nono/ds/dynamic-li-chao-tree.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    DynamicLiChaoTree<long long, int> tree;
    for (int i = 0; i < n; i++) {
        long long a, b;
        std::cin >> a >> b;
        tree.add_line(a, b);
    }
    for (int i = 0; i < q; i++) {
        int t;
        std::cin >> t;
        if (t == 0) {
            long long a, b;
            std::cin >> a >> b;
            tree.add_line(a, b);
        } else {
            long long p;
            std::cin >> p;
            std::cout << *tree.get(p) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
