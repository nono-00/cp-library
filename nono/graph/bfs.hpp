#pragma once

#include <limits>
#include <queue>
#include <vector>
#include <algorithm>

#include "nono/graph/base.hpp"

namespace nono {

namespace internal {

template <class T>
class BfsResult {
  public:
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();

    BfsResult(std::vector<T> dist, std::vector<int> parent): dist_(std::move(dist)), parent_(std::move(parent)) {}

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

///  brief : 幅優先探索. 使ったことがない.
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

template <class T>
internal::BfsResult<T> bfs(const Graph<T>& graph, int source) {
    return bfs(graph, std::vector<int>{source});
}

}  //  namespace nono
