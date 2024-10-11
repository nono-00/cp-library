#define PROBLEM "https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum_fixed_root"
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/math/modint.hpp"
#include "nono/tree/dynamic-tree-dp.hpp"

namespace nono {

using Mint = Modint998244353;
struct Info {
    struct Point {
        Point(): sum(0), num(0) {}
        Point(Mint sum, Mint num = 1): sum(sum), num(num) {}
        Mint sum;
        Mint num;
    };
    struct Edge {
        Edge(Mint a = 1, Mint b = 0): a(a), b(b) {}
        Mint a;
        Mint b;
    };
    struct Path {
        Point point{};
        Edge edge{};
    };
    //  light edge をつける作業
    static Point add_edge(Edge edge, Point point) {
        return {edge.a * point.sum + edge.b * point.num, point.num};
    }
    static Path add_vertex(Point real, Point vir) {
        return {Point{real.sum + vir.sum, real.num + vir.num}, Edge{}};
    }
    static Path vertex(Point point) {
        return {point, Edge{}};
    }
    //  rhs <- (mhs) <- lhs <- ... root
    static Path compress(Path lhs, Edge mhs, Path rhs) {
        Edge edge{lhs.edge.a * mhs.a, lhs.edge.a * mhs.b + lhs.edge.b};
        Point point{lhs.point.sum + edge.a * rhs.point.sum + edge.b * rhs.point.num, lhs.point.num + rhs.point.num};
        return {point, Edge{edge.a * rhs.edge.a, edge.a * rhs.edge.b + edge.b}};
    }
    static Point rake(Point lhs, Point rhs) {
        return {lhs.sum + rhs.sum, lhs.num + rhs.num};
    }
};

void solve() {
    using Point = Info::Point;
    using Edge = Info::Edge;
    int n, q;
    std::cin >> n >> q;
    std::vector<Point> a(n);
    for (auto& v: a) std::cin >> v.sum, v.num = 1;
    std::vector<EdgeBase<int>> edges;
    std::vector<Edge> w;
    for (int i = 0; i < n - 1; i++) {
        int u, v;
        Mint a, b;
        std::cin >> u >> v >> a >> b;
        edges.emplace_back(u, v);
        w.emplace_back(a, b);
    }
    DynamicTreeDP<Info> tree(n, edges, a, w);
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int u;
            Mint x;
            std::cin >> u >> x;
            tree.set_vertex(u, Point{x, 1});
        } else {
            int i;
            Mint x, y;
            std::cin >> i >> x >> y;
            tree.set_edge(i, Edge{x, y});
        }
        std::cout << tree.all_prod().sum << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
