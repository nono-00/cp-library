#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/5/DSL_5_B"
#include <algorithm>
#include <iostream>

#include "../../../nono/ds/fenwick-tree-2d.hpp"

namespace nono {

void solve() {
    constexpr int N = 1005;
    FenwickTree2D<int> fenwick(N, N);
    int n;
    std::cin >> n;
    while (n--) {
        int lh, lw, rh, rw;
        std::cin >> lh >> lw >> rh >> rw;
        fenwick.add(rh, rw, 1);
        fenwick.add(rh, lw, -1);
        fenwick.add(lh, rw, -1);
        fenwick.add(lh, lw, 1);
    }
    int ans = 0;
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            ans = std::max(ans, fenwick.sum(i, j));
        }
    }
    std::cout << ans << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
