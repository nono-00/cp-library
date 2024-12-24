#pragma once

#include <cassert>
#include <vector>

#include "../graph/base.hpp"
#include "./is-tree.hpp"

namespace nono {

//  これをコピペしていい感じに埋める
struct ReRootingInfoTemplate {
    //  各頂点が持つ値だったり
    using Value = int;
    //  辺重み
    using Edge = int;
    //  mergeの単位元
    Value e();
    //  subtree同士のmerge
    Value merge(Value lhs, Value rhs);
    //  辺重みを加える
    Value add_edge(Edge edge, Value vertex);
    //  virtual vertex に頂点重みを与える
    Value add_vertex(Value vertex, Value value);
};

///  brief : 全方位木dp

//  Info: 上のReRootingInfoTemplateを埋めてくれ
template <class Info>
std::vector<typename Info::Value> rerooting(const Graph<typename Info::Edge>& graph,
                                            const std::vector<typename Info::Value>& vertex) {
    using Value = Info::Value;
    assert(is_tree(graph));
    int n = graph.size();
    std::vector<Value> dp(n, Info::e());
    {
        auto dfs = [&](auto&& self, int u, int p) -> Value {
            Value value = Info::e();
            for (auto e: graph[u]) {
                if (e.to == p) continue;
                value = Info::merge(value, Info::add_edge(e.weight, self(self, e.to, u)));
            }
            return dp[u] = Info::add_vertex(vertex[u], value);
        };
        dfs(dfs, 0, -1);
    }
    {
        auto dfs = [&](auto&& self, int u, int p, Value p_value) -> void {
            int m = graph[u].size();
            std::vector<Value> prefix(m + 1, Info::e()), suffix(m + 1, Info::e());
            for (int i = 0; auto e: graph[u]) {
                prefix[i + 1] = suffix[i] = Info::add_edge(e.weight, e.to == p ? p_value : dp[e.to]);
                i++;
            }
            for (int i = 0; i < m; i++) prefix[i + 1] = Info::merge(prefix[i], prefix[i + 1]);
            for (int i = m; i--;) suffix[i] = Info::merge(suffix[i], suffix[i + 1]);
            dp[u] = Info::add_vertex(vertex[u], prefix[m]);
            for (int i = 0; auto e: graph[u]) {
                if (e.to != p) self(self, e.to, u, Info::add_vertex(vertex[u], Info::merge(prefix[i], suffix[i + 1])));
                i++;
            }
        };
        dfs(dfs, 0, -1, Info::e());
    }
    return dp;
}

}  //  namespace nono
