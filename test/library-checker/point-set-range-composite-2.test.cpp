#define PROBLEM "https://judge.yosupo.jp/problem/point_set_range_composite"
#include <iostream>
#include <vector>

#include "data-structure/dynamic-segment-tree.hpp"
#include "math/modint.hpp"

namespace nono {

using mint = Modint<998244353>;

template <class T>
struct Line {
    T slope;
    T constant;

    Line() = default;
    Line(const T& slope, const T& constant): slope(slope), constant(constant) {}

    T eval(const T& x) {
        return x * slope + constant;
    }
};

void solve() {
    using namespace nono;
    int n, q;
    std::cin >> n >> q;

    auto op = [](const Line<mint>& lhs, const Line<mint>& rhs) -> Line<mint> {
        return Line<mint>(lhs.slope * rhs.slope, rhs.slope * lhs.constant + rhs.constant);
    };
    auto e = []() -> Line<mint> {
        return Line<mint>(1, 0);
    };

    DynamicSegmentTree<Line<mint>, op, e> segtree;
    for (int i = 0; i < n; i++) {
        int a, b;
        std::cin >> a >> b;
        segtree.set(i, Line<mint>(a, b));
    }

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int p, c, d;
            std::cin >> p >> c >> d;
            segtree.set(p, Line<mint>(c, d));
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
    int t = 1;
    //  std::cin >> t;
    while (t--) nono::solve();
}
