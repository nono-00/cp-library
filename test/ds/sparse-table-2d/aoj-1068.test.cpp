#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/1068"

#include <iostream>
#include <limits>
#include <vector>

#include "nono/data-structure/sparse-table-2d.hpp"

namespace nono {

template <class T>
struct Min {
    using value_type = T;
    static value_type op(value_type lhs, value_type rhs) {
        return std::min(lhs, rhs);
    }
    static value_type e() {
        return std::numeric_limits<T>::max();
    }
};

bool solve() {
    int h, w, q;
    std::cin >> h >> w >> q;
    if (h == 0) return 1;
    std::vector a(h, std::vector<int>(w));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            std::cin >> a[i][j];
        }
    }
    SparseTable2D<Min<int>> table(a);
    while (q--) {
        int h1, w1, h2, w2;
        std::cin >> h1 >> w1 >> h2 >> w2;
        h2++;
        w2++;
        std::cout << table.prod(h1, w1, h2, w2) << '\n';
    }
    return 0;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    while (!nono::solve())
        ;
}
