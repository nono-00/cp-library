#pragma once

#include <vector>

namespace nono {

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
template <class GraphType>
std::vector<int> topological_sort(const GraphType& graph) {
    int n = graph.size();
    std::vector<int> indegree = graph.indegree();
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
        for (const auto& edge: graph[cur]) {
            indegree[edge.to]--;
            if (indegree[edge.to] == 0) {
                stack.push_back(edge.to);
            }
        }
    }

    return result;
}

}  //  namespace nono
