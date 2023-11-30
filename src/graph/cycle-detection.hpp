#pragma once

#include <vector>

namespace nono {

namespace internal {

//  brief:
//  - グラフの各頂点の状態を表す列挙型.
//
//  details:
//  - `NOT_VISITED`: 未訪問.
//  - `VISITED`: 訪問済み, かつ探索中.
//  - `FINISHED`: 訪問済み, かつ探索済み.
enum class State {
    NOT_VISITED,
    VISITED,
    FINISHED
};

}  //  namespace internal

//  brief:
//  - サイクル検出. 無向, 有向, 単純, 非単純問わず実行可能.
//
//  complexity:
//  - O(N + M)
//
//  return:
//  - サイクルが存在するならば, そのうち一つのサイクルを構成する辺の配列.
//  - そうでなければ、空の辺の配列.
template <class GraphType>
auto cycle_detection(const GraphType& graph) {
    using internal::State;
    using EdgeType = GraphType::EdgeType;
    constexpr int NONE = -1;

    std::vector<State> state(graph.size(), State::NOT_VISITED);
    std::vector<EdgeType> result;

    auto dfs = [&](const auto& self, int u, int edge_id) -> bool {
        state[u] = State::VISITED;
        for (const auto& e: graph[u]) {
            if (e.id == edge_id) continue;
            if (state[e.to] == State::NOT_VISITED) {
                if (self(self, e.to, e.id)) {
                    if (result.front().to != result.back().from) {
                        result.emplace_back(e);
                    }
                    return true;
                }
            } else if (state[e.to] == State::VISITED) {
                result.emplace_back(e);
                return true;
            }
        }
        state[u] = State::FINISHED;
        return false;
    };

    int n = graph.size();
    for (int i = 0; i < n; i++) {
        if (state[i] == State::NOT_VISITED) {
            if (dfs(dfs, i, NONE)) {
                std::ranges::reverse(result);
                return result;
            }
        }
    }

    return result;
}

}  //  namespace nono
