#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/2/DSL_2_G"
#include <iostream>

#include "nono/data-structure/range-add-fenwick-tree.hpp"

namespace nono {

template <class T>
struct Add {
    using value_type = T;
    static T op(T lhs, T rhs) {
        return lhs + rhs;
    }
    static T e() {
        return T(0);
    }
    static T inv(T elem) {
        return -elem;
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    RangeAddFenwickTree<Add<long long>> fenwick(n);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r, x;
            std::cin >> l >> r >> x;
            l--;
            fenwick.apply(l, r, x);
        } else {
            int l, r;
            std::cin >> l >> r;
            l--;
            std::cout << fenwick.prod(l, r) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
