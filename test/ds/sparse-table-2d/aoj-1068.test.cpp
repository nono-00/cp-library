#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/1068"

#include <iostream>
#include <vector>

#include "nono/ds/sparse-table-2d.hpp"
#include "nono/structure/monoid.hpp"

namespace nono {

bool solve() {
    int h, w, q;
    std::cin >> h >> w >> q;
    if (h == 0) return 1;
    std::vector a(h, std::vector<std::optional<int>>(w));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int v;
            std::cin >> v;
            a[i][j] = v;
        }
    }
    using Monoid = monoid::Min<int>;
    SparseTable2D<Monoid> table(a);
    while (q--) {
        int h1, w1, h2, w2;
        std::cin >> h1 >> w1 >> h2 >> w2;
        h2++;
        w2++;
        std::cout << table.prod(h1, w1, h2, w2).value() << '\n';
    }
    return 0;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    while (!nono::solve())
        ;
}
