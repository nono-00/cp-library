#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/1/DSL_1_B"
#include <iostream>

#include "../../../nono/ds/potentialized-union-find.hpp"
#include "../../../nono/structure/group.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    PotentializedUnionFind<group::Add<int>> puf(n);

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int x, y, z;
            std::cin >> x >> y >> z;
            puf.merge(x, y, z);
        } else {
            int x, y;
            std::cin >> x >> y;
            auto result = puf.potential(x, y);
            if (!result) {
                std::cout << '?' << std::endl;
            } else {
                std::cout << *result << std::endl;
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
