#define PROBLEM "https://judge.yosupo.jp/problem/unionfind_with_potential"
#include <iostream>

#include "nono/ds/potentialized-union-find.hpp"
#include "nono/math/modint.hpp"
#include "nono/structure/group.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    int n, q;
    std::cin >> n >> q;
    PotentializedUnionFind<group::Add<Mint>> puf(n);

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int x, y;
            Mint z;
            std::cin >> x >> y >> z;
            if (puf.merge(x, y, z)) {
                std::cout << 1 << '\n';
            } else if (puf.potential(x, y) == z) {
                std::cout << 1 << '\n';
            } else {
                std::cout << 0 << '\n';
            }
        } else {
            int x, y;
            std::cin >> x >> y;
            auto result = puf.potential(x, y);
            if (!result) {
                std::cout << -1 << std::endl;
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
