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

namespace dijkstra_internal {

//  # DijkstraResult
//  dijkstraの結果。各メンバ関数のdestが [0, n) でないと壊れる
template <class T>
class DijkstraResult {
  public:
    //  # UNREACHABLE
    //  到達できない頂点の距離として入る値
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();

    DijkstraResult(std::vector<T> dist, std::vector<int> parent): dist_(std::move(dist)), parent_(std::move(parent)) {}

    //  # dist(dest)
    //  始点からdestまでの距離。到達できない場合はUNREACHABLE
    //  O(1)
    T dist(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest];
    }

    //  # path(dest)
    //  始点からdestまでの最短経路を、頂点の列 (始点, ..., dest) で返す
    //  到達できない場合は空の列を返す
    //  O(|path|)
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

    //  # invalid(dest)
    //  destに到達できないならtrue
    //  O(1)
    bool invalid(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest] == UNREACHABLE;
    }

    //  # raw()
    //  全頂点の距離の配列を返す
    std::vector<T> raw() const {
        return dist_;
    }

  private:
    std::vector<T> dist_;
    //  parent_[i] = -1 => 始点、または到達できない頂点
    std::vector<int> parent_;
};

}  //  namespace dijkstra_internal

//  # dijkstra(graph, source)
//  複数始点のダイクストラ法。各頂点について、最も近い始点からの距離を求める
//  負の重みの辺があると壊れる。sourceの要素が [0, n) でないと壊れる
//  O((n + m) log (n + m))
template <class T>
dijkstra_internal::DijkstraResult<T> dijkstra(const Graph<T>& graph, std::vector<int> source) {
    using Result = dijkstra_internal::DijkstraResult<T>;

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

//  # dijkstra(graph, source)
//  単一始点のダイクストラ法
//  負の重みの辺があると壊れる。sourceが [0, n) でないと壊れる
//  O((n + m) log (n + m))
template <class T>
dijkstra_internal::DijkstraResult<T> dijkstra(const Graph<T>& graph, int source) {
    return dijkstra(graph, std::vector<int>{source});
}

}  //  namespace nono
