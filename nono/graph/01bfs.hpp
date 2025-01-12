#pragma once

#include <algorithm>
#include <deque>
#include <limits>
#include <vector>

#include "./base.hpp"

namespace nono {

namespace internal {

///  # 01bfs result
template <class T>
class Bfs01Result {
  public:
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();

    Bfs01Result(std::vector<T> dist, std::vector<int> parent): dist_(std::move(dist)), parent_(std::move(parent)) {}

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

///  # 01bfs(graph, sources)
///  複数始点01bfs
///  O(n + m)
template <class T>
internal::Bfs01Result<T> bfs01(const Graph<T>& graph, const std::vector<int>& source) {
    using Result = internal::Bfs01Result<T>;

    std::vector<T> dist(graph.size(), Result::UNREACHABLE);
    std::vector<int> parent(graph.size(), -1);
    std::vector<bool> used(graph.size());
    std::deque<int> que;
    for (auto s: source) {
        dist[s] = 0;
        que.push_back(s);
    }

    while (!que.empty()) {
        auto u = que.front();
        que.pop_front();
        if (used[u]) continue;
        used[u] = true;
        for (const auto& e: graph[u]) {
            assert(e.weight == 0 || e.weight == 1);
            if (dist[e.to] > dist[u] + e.weight) {
                dist[e.to] = dist[u] + e.weight;
                parent[e.to] = u;
                if (e.weight == 0) {
                    que.push_front(e.to);
                } else {
                    que.push_back(e.to);
                }
            }
        }
    }

    return Result(std::move(dist), std::move(parent));
}

///  # bfs01(graph, source)
///  単一始点01bfs
///  O(n + m)
template <class T>
internal::Bfs01Result<T> bfs01(const Graph<T>& graph, int source) {
    return bfs01(graph, std::vector<int>{source});
}

}  //  namespace nono
