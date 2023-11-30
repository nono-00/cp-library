#define PROBLEM "https://judge.yosupo.jp/problem/point_set_range_composite"
#include <iostream>
#include <vector>

#include <atcoder/modint>

#include "data-structure/segment-tree.hpp"

using mint = atcoder::modint998244353;

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

int main() {
    using namespace nono;
    int n, q;
    std::cin >> n >> q;
    std::vector<Line<mint>> lines(n);
    for (int i = 0; i < n; i++) {
        int a, b;
        std::cin >> a >> b;
        lines[i] = Line<mint>(a, b);
    }

    auto op = [](const Line<mint>& lhs, const Line<mint>& rhs) -> Line<mint> {
        return Line<mint>(lhs.slope * rhs.slope, rhs.slope * lhs.constant + rhs.constant);
    };

    auto e = []() -> Line<mint> {
        return Line<mint>(1, 0);
    };

    SegmentTree<Line<mint>, op, e> segtree(lines);

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
            std::cout << segtree.prod(l, r).eval(x).val() << std::endl;
        }
    }
}
