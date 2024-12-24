#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "./base.hpp"

namespace nono {

namespace internal {

template <class T>
class DijkstraResult {
  public:
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();

    DijkstraResult(std::vector<T> dist, std::vector<int> parent): dist_(std::move(dist)), parent_(std::move(parent)) {}

    //  source -> destの最短距離
    T dist(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest];
    }

    //  source -> destの最短経路
    //
    //  dist(dest)と違って辿り着けない/距離が確定しない頂点に対して
    //  呼び足すと空の配列を返す
    //
    //  source -> ... -> ... -> dest順
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

    //  辿り着けないかどうか
    bool invalid(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest] == UNREACHABLE;
    }

    std::vector<T> raw() const {
        return dist_;
    }

  private:
    std::vector<T> dist_;
    //  parent_[i] = -1 => source or unrechable
    std::vector<int> parent_;
};

}  //  namespace internal

///  brief : 負辺なしの単一始点最短経路問題を解く. \\( O(|E| \log |E|) \\)

//  brief:
//  - 負辺無し単一始点最短経路問題を解く
//
//  complexity:
//  - O((V + E)logV)
//
//  tparam:
//  - `GraphType`: グラフ型
//
//  return:
//  - `source` からたどり着くことができるのならば, `source` からの最短経路
//  - そうでなければ `numeric_limit<T>::max()`
template <class T>
internal::DijkstraResult<T> dijkstra(const Graph<T>& graph, std::vector<int> source) {
    using Result = internal::DijkstraResult<T>;

    std::vector<T> dist(graph.size(), Result::UNREACHABLE);
    std::vector<int> parent(graph.size(), -1);
    std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>, std::greater<std::pair<T, int>>> que;
    for (auto s: source) {
        dist[s] = 0;
        que.emplace(dist[s], s);
    }

    while (!que.empty()) {
        auto [d, u] = que.top();
        que.pop();
        if (dist[u] < d) continue;
        for (const auto& e: graph[u]) {
            if (dist[e.to] > dist[u] + e.weight) {
                dist[e.to] = dist[u] + e.weight;
                parent[e.to] = u;
                que.emplace(dist[e.to], e.to);
            }
        }
    }

    return Result(std::move(dist), std::move(parent));
}

template <class T>
internal::DijkstraResult<T> dijkstra(const Graph<T>& graph, int source) {
    return dijkstra(graph, std::vector<int>{source});
}

}  //  namespace nono
