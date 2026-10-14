#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "./base.hpp"

namespace nono {

namespace bellman_ford_internal {

//  # BellmanFordResult
//  bellman_fordの結果。各メンバ関数のdestが [0, n) でないと壊れる
template <class T>
class BellmanFordResult {
  public:
    //  # UNREACHABLE
    //  到達できない頂点の距離として入る値
    static constexpr T UNREACHABLE = std::numeric_limits<T>::max();
    //  # INVALID
    //  負閉路の影響で距離が定まらない頂点の距離として入る値
    static constexpr T INVALID = std::numeric_limits<T>::min();

    BellmanFordResult(std::vector<T> dist, std::vector<int> parent, bool has_negative_cycle)
        : dist_(std::move(dist)),
          parent_(std::move(parent)),
          has_negative_cycle_(has_negative_cycle) {}

    //  # dist(dest)
    //  始点からdestまでの距離
    //  到達できない場合はUNREACHABLE、負閉路の影響で定まらない場合はINVALID
    //  O(1)
    T dist(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest];
    }

    //  # path(dest)
    //  始点からdestまでの最短経路を、頂点の列 (始点, ..., dest) で返す
    //  invalid(dest) がtrueの場合は空の列を返す
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
    //  destに到達できない、または負閉路の影響で距離が定まらないならtrue
    bool invalid(int dest) const {
        assert(0 <= dest && dest < dist_.size());
        return dist_[dest] == UNREACHABLE || dist_[dest] == INVALID;
    }

    //  # has_negative_cycle()
    //  始点から到達できる負閉路があるか
    bool has_negative_cycle() const {
        return has_negative_cycle_;
    }

    //  # raw()
    //  全頂点の距離の配列を返す
    std::vector<T> raw() const {
        return dist_;
    }

  private:
    std::vector<T> dist_;
    std::vector<int> parent_;
    bool has_negative_cycle_;
};

}  //  namespace bellman_ford_internal

//  # bellman_ford(n, edges, source)
//  複数始点のベルマンフォード法。負の重みの辺があってもよい。nは頂点数
//  各頂点について、最も近い始点からの距離を求める
//  始点から到達できる負閉路があると、それを経由して到達できる頂点の距離はINVALIDになる
//  sourceの要素が [0, n) でないと壊れる。edgesの端点が [0, n) でないと壊れる
//  O(nm)
template <class T>
bellman_ford_internal::BellmanFordResult<T> bellman_ford(int n, const std::vector<EdgeBase<T>>& edges,
                                                         const std::vector<int>& source) {
    using Result = bellman_ford_internal::BellmanFordResult<T>;
    constexpr T INF = std::numeric_limits<T>::max();
    std::vector<T> dist(n, Result::UNREACHABLE);
    std::vector<int> parent(n, -1);
    for (auto s: source) {
        assert(0 <= s && s < n);
        dist[s] = 0;
    }

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

    return Result(std::move(dist), std::move(parent), has_negative_cycle);
}

//  # bellman_ford(n, edges, source)
//  単一始点のベルマンフォード法。内容は複数始点の版と同じ
//  O(nm)
template <class T>
bellman_ford_internal::BellmanFordResult<T> bellman_ford(int n, const std::vector<EdgeBase<T>>& edges, int source) {
    return bellman_ford(n, edges, std::vector<int>{source});
}

}  //  namespace nono
