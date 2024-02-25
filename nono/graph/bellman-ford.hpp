#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

//  bellman-fordの結果を表す型
//
//  コンストラクタはprivateなので呼び出せないようになっている
template <class T>
class BellmanFordResult {
  public:
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();
    static constexpr T INVALID = std::numeric_limits<T>::min();

    template <class U>
    friend BellmanFordResult<U> bellman_ford(int n, const std::vector<EdgeBase<U>>& edges, int source);

    //  source -> destの最短距離
    T dist(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest];
    }

    //  source -> destの最短経路
    //
    //  dist(dest)と違って辿り着けない/距離が確定しない頂点に対して
    //  呼び足すと壊れる
    //
    //  source -> ... -> ... -> dest順
    std::vector<int> path(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        assert(!invalid(dest));
        std::vector<int> result;
        while (true) {
            result.push_back(dest);
            if (dest == source_) break;
            dest = parent_[dest];
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    //  辿り着けないまたは距離が確定するかどうか
    bool invalid(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest] == UNREACHABLE || dist_[dest] == INVALID;
    }

    bool has_negative_cycle() const {
        return has_negative_cycle_;
    }

  private:
    BellmanFordResult(int source, std::vector<T>&& dist, std::vector<int>&& parent, bool has_negative_cycle)
        : source_(source),
          dist_(dist),
          parent_(parent),
          has_negative_cycle_(has_negative_cycle) {}

    int source_;
    std::vector<T> dist_;
    std::vector<int> parent_;
    bool has_negative_cycle_;
};

//  負辺ありの単一始点最短経路問題を解く
//
//  最短距離が確定しない頂点はstd::numeric_limits<T>::min()
//  辿り着けない頂点はstd::numeric_limits<T>::max()
template <class T>
BellmanFordResult<T> bellman_ford(int n, const std::vector<EdgeBase<T>>& edges, int source) {
    using Result = BellmanFordResult<T>;
    constexpr T INF = std::numeric_limits<T>::max();
    assert(0 <= source && source < n);
    std::vector<T> dist(n, Result::UNREACHABLE);
    std::vector<int> parent(n, source);
    dist[source] = 0;

    for (int i = 0; i + 1 < n; i++) {
        bool update = false;
        for (const auto& e: edges) {
            if (dist[e.from] == Result::UNREACHABLE) continue;
            if (dist[e.to] > dist[e.from] + e.weight) {
                update = true;
                dist[e.to] = dist[e.from] + e.weight;
                parent[e.to] = e.from;
            }
        }
        if (!update) break;
    }
    bool has_negative_cycle = false;
    for (int i = 0; i < n; i++) {
        bool update = false;
        for (const auto& e: edges) {
            if (dist[e.from] == Result::UNREACHABLE) continue;
            if (dist[e.from] == Result::INVALID || dist[e.to] > dist[e.from] + e.weight) {
                update = true;
                dist[e.to] = Result::INVALID;
            }
        }
        if (!update) break;
        has_negative_cycle = true;
    }

    return Result(source, std::move(dist), std::move(parent), has_negative_cycle);
}

}  //  namespace nono
