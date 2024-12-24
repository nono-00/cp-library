#pragma once

#include <algorithm>
#include <deque>
#include <limits>
#include <vector>

#include "./base.hpp"

namespace nono {

namespace internal {

template <class T>
class Bfs01Result {
  public:
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();

    Bfs01Result(std::vector<T> dist, std::vector<int> parent): dist_(std::move(dist)), parent_(std::move(parent)) {}

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

///  brief : 01bfs.
//  verified https://www.acmicpc.net/problem/13549
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

template <class T>
internal::Bfs01Result<T> bfs01(const Graph<T>& graph, int source) {
    return bfs01(graph, std::vector<int>{source});
}

}  //  namespace nono
