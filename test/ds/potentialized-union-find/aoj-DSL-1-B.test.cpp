#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/1/DSL_1_B"
#include <iostream>

#include "nono/data-structure/potentialized-union-find.hpp"

namespace nono {

struct Add {
    using value_type = int;
    static value_type op(value_type lhs, value_type rhs) {
        return lhs + rhs;
    }
    static value_type e() {
        return 0;
    }
    static value_type inv(value_type value) {
        return -value;
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    PotentializedUnionFind<Add> puf(n);

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
            if (result.invalid()) {
                std::cout << '?' << std::endl;
            } else {
                std::cout << result.potential() << std::endl;
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
