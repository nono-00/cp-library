#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "./base.hpp"

namespace nono {

//  # topological_sort(graph)
//  トポロジカルソートした頂点の列を1つ返す。閉路があれば空の列を返す
//  有向グラフでないと壊れる
//  O(n + m)
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
    if (std::ssize(result) != n) return {};
    return result;
}

}  //  namespace nono
