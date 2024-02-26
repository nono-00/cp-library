#define PROBLEM "https://judge.yosupo.jp/problem/point_set_range_composite"
#include <iostream>
#include <vector>

#include "nono/data-structure/segment-tree.hpp"
#include "nono/math/modint.hpp"

namespace nono {

using Mint = nono::Modint998244353;

template <class T>
struct Line {
    T slope;
    T constant;

    Line() = default;
    Line(T slope, T constant): slope(slope), constant(constant) {}

    T eval(T x) {
        return x * slope + constant;
    }
};

template <class T>
struct Composite {
    using value_type = Line<T>;
    static value_type op(value_type lhs, value_type rhs) {
        return Line<Mint>(lhs.slope * rhs.slope, rhs.slope * lhs.constant + rhs.constant);
    }
    static value_type e() {
        return Line<Mint>(1, 0);
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<Line<Mint>> lines(n);
    for (int i = 0; i < n; i++) {
        int a, b;
        std::cin >> a >> b;
        lines[i] = Line<Mint>(a, b);
    }

    SegmentTree<Composite<Mint>> segtree(lines);
    using Data = Composite<Mint>::value_type;

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p, c, d;
            std::cin >> p >> c >> d;
            segtree.set(p, Data(c, d));
        } else {
            int l, r, x;
            std::cin >> l >> r >> x;
            std::cout << segtree.prod(l, r).eval(x) << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
