#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc351/tasks/abc351_g"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/math/modint.hpp"
#include "nono/tree/dynamic-tree-dp.hpp"

namespace nono {

using Mint = Modint998244353;

struct Info {
    //  (k, c)
    using Point = std::pair<Mint, Mint>;
    using Edge = bool;
    struct Path {
        Point point{};
        Edge edge{};
    };
    //  light edge をつける作業
    static Point add_edge(Edge edge, Point point) {
        return {point.second, 0};
    }
    //  virtual point にreal を付与
    static Path add_vertex(Point real, Point vir) {
        assert(vir.second == 0);
        assert(real.first == 1);
        return {Point{vir.first, real.second}, false};
    }
    //  単一要素の頂点
    static Path vertex(Point point) {
        return Path{point, false};
    }
    //  rhs <- (mhs) <- lhs <- ... root
    static Path compress(Path lhs, Edge mhs, Path rhs) {
        return {Point{lhs.point.first * rhs.point.first, lhs.point.second + lhs.point.first * rhs.point.second}, false};
    }
    //  同一のvirtual root を持つやつを merge
    static Point rake(Point lhs, Point rhs) {
        assert(lhs.second == 0);
        assert(rhs.second == 0);
        return Point{lhs.first * rhs.first, 0};
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    std::vector<Edge> edges;
    for (int i = 1; i < n; i++) {
        int p;
        std::cin >> p;
        p--;
        edges.emplace_back(p, i);
    }
    std::vector<std::pair<Mint, Mint>> a(n);
    for (auto& v: a) std::cin >> v.second, v.first = 1;
    DynamicTreeDP<Info> tree(n, edges, a, std::vector<bool>(n - 1));
    while (q--) {
        int u;
        Mint x;
        std::cin >> u >> x;
        u--;
        tree.set_vertex(u, {1, x});
        auto result = tree.all_prod();
        std::cout << result.second << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
