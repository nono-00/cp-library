#define PROBLEM "https://judge.yosupo.jp/problem/unionfind"
#include <iostream>

#include "data-structure/union-find.hpp"

int main() {
    int n, q;
    std::cin >> n >> q;
    nono::UnionFind uf(n);
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
