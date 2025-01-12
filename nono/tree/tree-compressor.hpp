#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "../ds/sparse-table.hpp"
#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

namespace internal {

///  # TreeCompressResult
///  graph := compressed graph
///  mapping[i] := vertex i (compressed graph) -> vertex mapping[i] (original graph)
template <class T>
struct TreeCompressResult {
    Graph<T> graph;
    std::vector<int> mapping;
};

}  //  namespace internal

///  # Tree Compressor
///  "指定された頂点たちの最小共通祖先関係を保って木を圧縮してできる補助的な木" を作るやつ
template <class T>
class TreeCompressor {
    using Result = internal::TreeCompressResult<T>;

    struct Min_ {
        using Value = std::pair<int, int>;
        static Value op(Value lhs, Value rhs) {
            return std::min(lhs, rhs);
        }
        static Value e() {
            return {1e9, 1e9};
        }
    };

  public:
    ///  # TreeCompressor(graph)
    ///  O(n log n)
    TreeCompressor(const Graph<T>& graph): in_(graph.size()), out_(graph.size()) {
        assert(is_tree(graph));
        int n = graph.size();
        std::vector<std::pair<int, int>> depth_index_pairs(2 * n);
        int now = 0;
        auto dfs = [&](auto self, int u, int p, int d) -> void {
            in_[u] = now;
            depth_index_pairs[now] = {d, u};
            now++;
            for (const auto& e: graph[u]) {
                if (e.to == p) continue;
                self(self, e.to, u, d + 1);
                depth_index_pairs[now] = {d, u};
                now++;
            }
            out_[u] = now;
        };
        dfs(dfs, 0, -1, 0);
        table_ = SparseTable<Min_>(depth_index_pairs);
    }

    ///  # compress(vertices)
    ///  verticesに含まれる頂点の祖先関係を保ったtreeを作る
    ///  O(|v| log |v|)
    Result compress(std::vector<int> vertices) const {
        std::ranges::sort(vertices, [&](int lhs, int rhs) { return in_[lhs] < in_[rhs]; });
        for (int i = 0, n = vertices.size(); i + 1 < n; i++) vertices.push_back(lca(vertices[i], vertices[i + 1]));
        std::ranges::sort(vertices, [&](int lhs, int rhs) { return in_[lhs] < in_[rhs]; });
        vertices.erase(std::unique(vertices.begin(), vertices.end()), vertices.end());
        int n = vertices.size();
        std::vector<int> stack;
        std::vector<Edge> edges;
        stack.push_back(0);
        for (int i = 1; i < n; i++) {
            int u = vertices[i];
            while (!stack.empty()) {
                int v = vertices[stack.back()];
                if (out_[u] < out_[v]) break;
                stack.pop_back();
            }
            assert(!stack.empty());
            edges.emplace_back(stack.back(), i);
            stack.push_back(i);
        }
        assert((int)edges.size() == n - 1);
        return Result(to_undirected_graph(n, edges), vertices);
    }

  private:
    int lca(int u, int v) const {
        if (in_[u] <= in_[v] && out_[v] <= out_[u]) return u;
        if (in_[v] <= in_[u] && out_[u] <= out_[v]) return v;
        if (in_[u] > in_[v]) std::swap(u, v);
        assert(out_[u] < in_[v] + 1);
        return table_.prod(out_[u], in_[v] + 1).second;
    }

    std::vector<int> in_;
    std::vector<int> out_;
    SparseTable<Min_> table_;
};

}  //  namespace nono
