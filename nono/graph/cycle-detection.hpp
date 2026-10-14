#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

#include "./base.hpp"

namespace nono {

namespace cycle_detection_internal {

//  探索中の各頂点の状態
//  NOT_VISITED : 未訪問
//  VISITED     : 訪問済みで、探索中
//  FINISHED    : 訪問済みで、探索も終わっている
enum class State {
    NOT_VISITED,
    VISITED,
    FINISHED
};

//  # CycleDetectionResult
//  cycle_detectionの結果。見つけた閉路を1つ持つ
//  v[0] e[0] v[1] e[1] ... v[k - 1] e[k - 1] v[0] の順に辿る閉路で、
//  e[i] は v[i] から v[i + 1] へ向かう辺
class CycleDetectionResult {
  public:
    CycleDetectionResult(std::vector<int> edge_id, std::vector<int> vertex_id)
        : edge_id_(std::move(edge_id)),
          vertex_id_(std::move(vertex_id)) {
        assert(edge_id_.size() == vertex_id_.size());
    }

    //  # has_cycle()
    //  閉路が見つかったか
    bool has_cycle() const {
        return !edge_id_.empty();
    }

    //  # edge_id()
    //  閉路の辺の番号の列 e[0], ..., e[k - 1] を返す
    //  閉路が無いと壊れる
    std::vector<int> edge_id() const {
        assert(has_cycle());
        return edge_id_;
    }

    //  # vertex_id()
    //  閉路の頂点の列 v[0], ..., v[k - 1] を返す
    //  閉路が無いと壊れる
    std::vector<int> vertex_id() const {
        assert(has_cycle());
        return vertex_id_;
    }

  private:
    std::vector<int> edge_id_;
    std::vector<int> vertex_id_;
};

}  //  namespace cycle_detection_internal

//  # cycle_detection(graph)
//  閉路を1つ見つける。無向グラフでも有向グラフでもよく、多重辺や自己ループがあってもよい
//  無向グラフの多重辺は長さ2の閉路、自己ループは長さ1の閉路として見つかる
//  O(n + m)
template <class T>
auto cycle_detection(const Graph<T>& graph) {
    using cycle_detection_internal::State;
    using Result = cycle_detection_internal::CycleDetectionResult;
    constexpr int NONE = -1;

    std::vector<State> state(graph.size(), State::NOT_VISITED);
    std::vector<int> edge_history, vertex_history;

    auto dfs = [&](const auto& self, int u, int edge_id) -> int {
        if (state[u] == State::VISITED) return u;
        vertex_history.push_back(u);
        state[u] = State::VISITED;
        for (const auto& e: graph[u]) {
            if (e.id == edge_id) continue;
            if (state[e.to] == State::FINISHED) continue;
            edge_history.push_back(e.id);
            auto res = self(self, e.to, e.id);
            if (res != NONE) return res;
            edge_history.pop_back();
        }
        vertex_history.pop_back();
        state[u] = State::FINISHED;
        return NONE;
    };

    int n = graph.size();
    for (int i = 0; i < n; i++) {
        if (state[i] == State::NOT_VISITED) {
            int start = dfs(dfs, i, NONE);
            if (start != NONE) {
                int length = std::distance(std::ranges::find(vertex_history, start), vertex_history.end());
                assert(length > 0);
                return Result(std::vector(edge_history.end() - length, edge_history.end()),
                              std::vector(vertex_history.end() - length, vertex_history.end()));
            }
        }
    }
    return Result({}, {});
}

}  //  namespace nono
