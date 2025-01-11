#define PROBLEM "https://judge.yosupo.jp/problem/range_reverse_range_sum"
#include <iostream>

#include "../../../nono/ds/treap.hpp"
#include "../../../nono/structure/monoid.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    nono::Treap<monoid::Add<long long>> treap;
    for (int i = 0; i < n; i++) {
        long long value;
        std::cin >> value;
        treap.push_back(value);
    }
    while (q--) {
        int t, l, r;
        std::cin >> t >> l >> r;
        if (t == 0) {
            treap.reverse(l, r);
        } else {
            std::cout << treap.prod(l, r) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
