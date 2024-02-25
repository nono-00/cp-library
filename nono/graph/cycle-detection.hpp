#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "nono/graph/base.hpp"

namespace nono {

namespace internal {

//  グラフの各頂点の状態を表す列挙型.
//
//  `NOT_VISITED`: 未訪問.
//  `VISITED`: 訪問済み, かつ探索中.
//  `FINISHED`: 訪問済み, かつ探索済み.
enum class State {
    NOT_VISITED,
    VISITED,
    FINISHED
};

//  cycle-detectionの結果
//
//  v[0] e[0] v[1] e[1] ... v[n - 1] e[n - 1] v[0] ...
//  のような感じになっている
class CycleDetectionResult {
  public:
    CycleDetectionResult(std::vector<int> edge_id, std::vector<int> vertex_id)
        : edge_id_(std::move(edge_id)),
          vertex_id_(std::move(vertex_id)) {
        assert(edge_id_.size() == vertex_id_.size());
    }

    bool has_cycle() const {
        return !edge_id_.empty();
    }

    std::vector<int> edge_id() const {
        return edge_id_;
    }

    std::vector<int> vertex_id() const {
        return vertex_id_;
    }

  private:
    std::vector<int> edge_id_;
    std::vector<int> vertex_id_;
};

}  //  namespace internal

//  サイクル検出. 無向, 有向, 単純, 非単純問わず実行可能.
template <class T>
auto cycle_detection(const Graph<T>& graph) {
    using internal::State;
    using Result = internal::CycleDetectionResult;
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
