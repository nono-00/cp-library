#define PROBLEM "https://judge.yosupo.jp/problem/unionfind"
#include <iostream>

#include "nono/ds/union-find.hpp"

namespace nono {

void solve() {
    int n, q;
    std::cin >> n >> q;
    UnionFind uf(n);
    while (q--) {
        int t, u, v;
        std::cin >> t >> u >> v;
        if (t == 0) {
            uf.merge(u, v);
        } else {
            std::cout << (uf.same(u, v) ? 1 : 0) << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
