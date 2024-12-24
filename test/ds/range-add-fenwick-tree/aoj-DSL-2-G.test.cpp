#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/2/DSL_2_G"
#include <iostream>

#include "../../../nono/ds/range-add-fenwick-tree.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    RangeAddFenwickTree<long long> fenwick(n);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r, x;
            std::cin >> l >> r >> x;
            l--;
            fenwick.add(l, r, x);
        } else {
            int l, r;
            std::cin >> l >> r;
            l--;
            std::cout << fenwick.sum(l, r) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
