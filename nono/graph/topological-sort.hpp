#pragma once

#include <cassert>
#include <vector>

#include "./base.hpp"

namespace nono {

///  # topological sort(graph)
///  if G is not DAG, return empty vector
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
    if ((int)result.size() != n) return {};
    return result;
}

}  //  namespace nono
