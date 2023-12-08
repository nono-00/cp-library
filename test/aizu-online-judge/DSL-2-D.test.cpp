#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/3/DSL/2/DSL_2_D"
#include <iomanip>
#include <iostream>
#include <limits>

#include "data-structure/dual-segment-tree.hpp"

namespace nono {

struct Data {
    Data() {}
    Data(int t, int v): timestamp(t), value(v) {}

    int timestamp;
    int value;
};

Data id() {
    return {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
}

Data mapping(Data f, Data v) {
    if (f.timestamp < v.timestamp) {
        return v;
    } else {
        return f;
    }
}

Data compsition(Data f, Data v) {
    if (f.timestamp < v.timestamp) {
        return v;
    } else {
        return f;
    }
}

Data e() {
    return {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
}

void solve() {
    int n, q;
    std::cin >> n >> q;
    DualSegmentTree<Data, e, Data, mapping, compsition, id> segtree(n);
    for (int i = 0; i < q; i++) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int l, r, x;
            std::cin >> l >> r >> x;
            r++;
            segtree.apply(l, r, {i, x});
        } else {
            int p;
            std::cin >> p;
            std::cout << segtree.get(p).value << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
