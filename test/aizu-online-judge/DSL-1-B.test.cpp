#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/1/DSL_1_B"
#include <iostream>
#include <limits>

#include "data-structure/potentialized-union-find.hpp"

int main() {
    constexpr int INF = std::numeric_limits<int>::max();
    using namespace nono;
    int n, q;
    std::cin >> n >> q;
    PotentializedUnionFind<int> puf(n);

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
            if (result == INF) {
                std::cout << '?' << std::endl;
            } else {
                std::cout << result << std::endl;
            }
        }
    }
}
