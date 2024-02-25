#pragma once

#include <algorithm>
#include <vector>

#include "nono/graph/internal-graph-concepts.hpp"

namespace nono {

//  brief:
//  - 強連結成分分解
//
//  complexity:
//  - O(V + E)
//
//  param:
//  - `graph`: 有向グラフ. 単純非単純を問わない
//
//  return:
//  - 強連結成分ごとの頂点配列をトポロジカルソートした配列
//
//  note:
//  - Tarjan's algorithmを使用している.
//  -
//  - Kosaraju's algorithmも実装した.
//  - コメントアウトしてあるが使える（はず）
template <internal::Graph GraphType>
std::vector<std::vector<int>> strongly_connected_components(const GraphType& graph) {
    constexpr int NONE = -1;

    int n = graph.size();
    std::vector<int> order(n, NONE);
    std::vector<int> group_ids(n, NONE);
    std::vector<int> count(n);
    std::vector<int> stack;
    stack.reserve(n);

    int now = 0;
    int group_id = 0;

    auto dfs = [&](const auto& self, int u) -> int {
        int low = now;
        order[u] = now;
        now++;
        stack.push_back(u);

        for (const auto& edge: graph[u]) {
            if (order[edge.to] == NONE) {
                low = std::min(low, self(self, edge.to));
            } else if (group_ids[edge.to] == NONE) {
                low = std::min(low, order[edge.to]);
            }
        }

        if (low == order[u]) {
            while (!stack.empty()) {
                int v = stack.back();
                if (order[v] < order[u]) break;
                stack.pop_back();
                group_ids[v] = group_id;
                count[group_id]++;
            }
            group_id++;
        }

        return low;
    };

    for (int i = 0; i < n; i++) {
        if (order[i] == NONE) {
            dfs(dfs, i);
        }
    }

    std::vector<std::vector<int>> groups(group_id);
    for (int i = 0; i < group_id; i++) {
        groups[i].reserve(count[i]);
    }
    for (int i = 0; i < n; i++) {
        groups[group_ids[i]].push_back(i);
    }
    std::reverse(groups.begin(), groups.end());
    return groups;
}

}  //  namespace nono

//  #pragma once
//
//  #include <algorithm>
//  #include <utility>
//  #include <vector>
//
//  namespace nono {
//
//  namespace internal::scc {
//
//  using SccGraph = std::vector<std::vector<int>>;
//
//  void dfs(const SccGraph& graph, int u, std::vector<int>& group, std::vector<bool>& discovered) {
//      discovered[u] = true;
//      for (int v: graph[u]) {
//          if (not discovered[v]) {
//              dfs(graph, v, group, discovered);
//          }
//      }
//      group.push_back(u);
//  }
//
//  template <class GraphType>
//  std::pair<SccGraph, SccGraph> to_scc_graph(const GraphType& graph) {
//      int n = graph.size();
//      SccGraph lhs(n), rhs(n);
//      for (int i = 0; i < n; i++) {
//          for (const auto& edge: graph[i]) {
//              lhs[edge.from].push_back(edge.to);
//              rhs[edge.to].push_back(edge.from);
//          }
//      }
//      return {lhs, rhs};
//  }
//
//  }  //  namespace internal::scc
//
//  template <class GraphType>
//  std::vector<std::vector<int>> strongly_connected_components(const GraphType& graph) {
//      int n = graph.size();
//      std::vector<int> finished;
//      std::vector<bool> discovered(n);
//      finished.reserve(n);
//      const auto [scc_graph, inv_scc_graph] = internal::scc::to_scc_graph(graph);
//
//      //  各頂点の探索が終わるタイミングを記録
//      for (int i = 0; i < n; i++) {
//          if (not discovered[i]) {
//              internal::scc::dfs(scc_graph, i, finished, discovered);
//          }
//      }
//
//      std::reverse(finished.begin(), finished.end());
//      discovered.assign(n, false);
//      std::vector<std::vector<int>> groups;
//
//      for (int i: finished) {
//          if (not discovered[i]) {
//              std::vector<int> group;
//              internal::scc::dfs(inv_scc_graph, i, group, discovered);
//              groups.emplace_back(group);
//          }
//      }
//
//      return groups;
//  }
//
//  }  //  namespace nono
