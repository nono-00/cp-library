#pragma once

#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

/// brief : トポソする. 
/// TODO : DAGかどうかassertion

//  brief:
//  - トポロジカルソート. 入次数が0の頂点を削っていく.
//
//  pre:
//  - グラフはDAGである必要がある
//
//  return:
//  - グラフがDAGの場合は、トポロジカルソートされた頂点配列
//
//  note:
//  - トポソ可能ならばグラフはDAGなので,
//  - `result.size() == graph.size()`ならばグラフがDAGであると判定できる
template <class T>
std::vector<int> topological_sort(const Graph<T>& graph) {
    assert(graph.is_directed());
    int n = graph.size();
    std::vector<int> indegree(n);
    for (int i = 0; i < n; i++) {
        for (const auto& e: graph[i]) {
            indegree[e.to]++;
        }
    }
    std::vector<int> stack;
    for (int i = 0; i < n; i++) {
        if (indegree[i] == 0) {
            stack.push_back(i);
        }
    }
    std::vector<int> result;
    result.reserve(n);
    while (!stack.empty()) {
        int cur = stack.back();
        stack.pop_back();
        result.push_back(cur);
        for (const auto& e: graph[cur]) {
            indegree[e.to]--;
            if (indegree[e.to] == 0) {
                stack.push_back(e.to);
            }
        }
    }
    return result;
}

}  //  namespace nono
