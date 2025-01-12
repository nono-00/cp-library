#pragma once

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

#include "./base.hpp"

namespace nono {

namespace internal {

template <class T>
class BfsResult {
  public:
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();

    BfsResult(std::vector<T> dist, std::vector<int> parent): dist_(std::move(dist)), parent_(std::move(parent)) {}

    ///  # dist[dest]
    ///  distance between source and dest
    ///  if source and dest are not connected, return UNREACHABLE
    ///  O(1)
    T dist(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest];
    }

    ///  # path(dest)
    ///  return shortest path from source to dest
    ///  source -> ... -> ... -> dest
    ///  if we can not reach dest, return empty vector
    ///  O(|path|)
    std::vector<int> path(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        if (invalid(dest)) {
            return {};
        }
        std::vector<int> result;
        for (int pos = dest; pos != -1;) {
            result.push_back(pos);
            pos = parent_[pos];
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    ///  # invalid(dest)
    ///  whether we can reach dest
    ///  O(1)
    bool invalid(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest] == UNREACHABLE;
    }

    ///  # raw()
    ///  return distance array
    std::vector<T> raw() const {
        return dist_;
    }

  private:
    std::vector<T> dist_;
    //  parent_[i] = -1 => source or unrechable
    std::vector<int> parent_;
};

}  //  namespace internal

///  # multiple source bfs
///  O(n)
template <class T>
internal::BfsResult<T> bfs(const Graph<T>& graph, const std::vector<int>& source) {
    using Result = internal::BfsResult<T>;
    std::vector<T> dist(graph.size(), Result::UNREACHABLE);
    std::vector<int> parent(graph.size(), -1);
    std::queue<int> que;
    for (auto s: source) {
        dist[s] = 0;
        que.push(s);
    }

    while (!que.empty()) {
        int u = que.front();
        que.pop();
        for (const auto& e: graph[u]) {
            if (dist[e.to] == Result::UNREACHABLE) {
                dist[e.to] = dist[u] + e.weight;
                parent[e.to] = u;
                que.push(e.to);
            }
        }
    }

    return Result{dist, parent};
}

///  # single source bfs
///  O(n)
template <class T>
internal::BfsResult<T> bfs(const Graph<T>& graph, int source) {
    return bfs(graph, std::vector<int>{source});
}

}  //  namespace nono
