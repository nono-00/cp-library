#define PROBLEM "https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum_fixed_root"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/math/modint.hpp"
#include "../../../nono/tree/static-top-tree.hpp"

namespace nono {

template <class Info>
class DynamicTreeDP {
    using Point = Info::Point;
    using Edge = Info::Edge;
    using Path = Info::Path;

    using NodeType = static_top_tree::NodeType;

  public:
    template <class T>
    DynamicTreeDP(int n, const std::vector<EdgeBase<T>>& init, const std::vector<Point>& vertexs,
                  const std::vector<Edge>& edges, int root = 0)
        : n_(n),
          stt_(to_static_top_tree(n, init, root)),
          tot_(stt_.nodes.size()),
          vertexs_(stt_.nodes.size()),
          edges_(stt_.nodes.size()) {
        for (int i = 0; i < n_; i++) {
            vertexs_[stt_.vertexs[i]] = vertexs[i];
        }
        for (int i = 0; i + 1 < n_; i++) {
            edges_[stt_.edges[i]] = edges[i];
        }
        dfs(stt_.root);
    }

    Point all_prod() {
        return tot_[stt_.root].point;
    }

    void set_edge(int i, Edge edge) {
        int u = stt_.edges[i];
        edges_[u] = edge;
        update(u);
        while (u != stt_.root) {
            u = stt_.nodes[u].parent;
            update(u);
        }
    }

    void set_vertex(int i, Point point) {
        int u = stt_.vertexs[i];
        vertexs_[u] = point;
        update(u);
        while (u != stt_.root) {
            u = stt_.nodes[u].parent;
            update(u);
        }
    }

  private:
    void dfs(int u) {
        if (stt_.nodes[u].left != -1) dfs(stt_.nodes[u].left);
        if (stt_.nodes[u].right != -1) dfs(stt_.nodes[u].right);
        update(u);
    }

    void update(int u) {
        const auto& node = stt_.nodes[u];
        switch (node.type) {
            case NodeType::AddEdge:
                tot_[u].point = Info::add_edge(edges_[u], tot_[node.left].point);
                break;
            case NodeType::AddVertex:
                tot_[u] = Info::add_vertex(vertexs_[u], tot_[node.left].point);
                break;
            case NodeType::Vertex:
                tot_[u] = Info::vertex(vertexs_[u]);
                break;
            case NodeType::Compress:
                tot_[u] = Info::compress(tot_[node.left], edges_[u], tot_[node.right]);
                break;
            case NodeType::Rake:
                tot_[u].point = Info::rake(tot_[node.left].point, tot_[node.right].point);
                break;
        };
    }

    int n_;
    StaticTopTree stt_;
    std::vector<Path> tot_;
    std::vector<Point> vertexs_;
    std::vector<Edge> edges_;
};

struct TreeDPInfo {
    using Point = int;
    using Edge = int;
    struct Path {
        Point point{};
        Edge edge{};
    };
    //  light edge をつける作業
    static Point add_edge(Edge edge, Point point);
    static Path add_vertex(Point real, Point vir);
    static Path vertex(Point point);
    //  rhs <- (mhs) <- lhs <- ... root
    static Path compress(Path lhs, Edge mhs, Path rhs);
    static Point rake(Point lhs, Point rhs);
};

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
