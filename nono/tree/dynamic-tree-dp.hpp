#pragma once

#include <vector>

#include "../graph/base.hpp"
#include "./static-top-tree.hpp"

namespace nono {

struct TreeDPInfoTemplate {
    using Point = int;
    using Edge = int;
    struct Path {
        Point point{};
        Edge edge{};
    };
    //  light edge をつける作業
    static Point add_edge(Edge edge, Point point);
    //  virtual point にreal を付与
    static Path add_vertex(Point real, Point vir);
    //  単一要素の頂点
    static Path vertex(Point point);
    //  rhs <- (mhs) <- lhs <- ... root
    static Path compress(Path lhs, Edge mhs, Path rhs);
    //  同一のvirtual root を持つやつを merge
    static Point rake(Point lhs, Point rhs);
};

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

}  //  namespace nono
