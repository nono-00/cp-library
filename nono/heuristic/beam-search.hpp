#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

#include "nono/heuristic/search-selector.hpp"
#include "nono/heuristic/search-tree.hpp"

namespace nono {

///  # BeamSearchConfig
struct BeamSearchConfig {
    ///  最大何手進めるか
    int max_turn;
    ///  各手で残す候補数
    int beam_width;
};

///  # BeamSearchResult
template <class Action>
struct BeamSearchResult {
    std::vector<Action> actions;
    double score;
};

namespace beam_search_internal {

///  探索木の同じ深さのノードは、id順に並べるとDFS順になる。
///  子は親を処理した順に連続して作られるので、深さd-1をid順に辿っていれば
///  深さdのidも(親の順, 子の順)の辞書順になっているため。
///  この順で辿ると、隣接するノード間の移動距離が最小になる
template <class Frontier>
void sort_by_id(Frontier& frontier) {
    std::sort(frontier.begin(), frontier.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
}

}  //  namespace beam_search_internal

///  # beam_search<Action, Hash>(state, score, config, expand, apply, undo)
///  単一のStateをEuler tourに沿って動かす版。ノードはActionしか持たないので、
///  メモリは O(生成ノード数 * sizeof(Action))。
///  applyとundoが互いに逆であることが必要
///
///  ## 利用者が渡すもの
///  expand(state, f) : 遷移候補ごとに f(action, score, hash) を呼ぶ。
///                     scoreは遷移後の絶対スコア、hashは遷移後の状態の重複排除キー。
///                     いずれも「次の状態を作らずに」求めること。
///                     一度もfを呼ばなければ、そのノードは終端として扱われる
///  apply(state, action) : stateにactionを適用する
///  undo(state, action)  : applyを打ち消す
///
///  ## Actionにはundoに必要な情報を持たせる
///  遷移で壊れるが復元が高価な値はActionに退避しておくとundoが軽くなる
template <class Action, class Hash, class State, class Expand, class Apply, class Undo>
BeamSearchResult<Action> beam_search(const State& initial_state, double initial_score, const BeamSearchConfig& config,
                                     Expand expand, Apply apply, Undo undo) {
    assert(config.beam_width > 0);
    SearchTree<Action> tree;
    BeamSelector<Hash> selector;
    State state = initial_state;
    int current = SearchTree<Action>::ROOT;

    //  (node id, score)
    std::vector<std::pair<int, double>> frontier{{SearchTree<Action>::ROOT, initial_score}};
    int best_id = SearchTree<Action>::ROOT;
    double best_score = std::numeric_limits<double>::lowest();

    for (int turn = 0; turn < config.max_turn; turn++) {
        selector.clear();
        beam_search_internal::sort_by_id(frontier);

        for (auto [id, score]: frontier) {
            tree.move_state(state, current, id, apply, undo);
            current = id;
            int produced = 0;
            expand(state, [&](const Action& action, double next_score, Hash hash) {
                produced++;
                selector.push(tree.add(id, action), hash, next_score);
            });
            //  展開先がないノードは終端。答えの候補として覚えておく
            if (produced == 0 && score > best_score) {
                best_score = score;
                best_id = id;
            }
        }

        const auto& selected = selector.select(config.beam_width);
        if (selected.empty()) break;
        frontier.clear();
        for (const auto& entry: selected) frontier.push_back({entry.id, entry.score});
    }

    //  最終的に残った候補も答えの候補
    for (auto [id, score]: frontier) {
        if (score > best_score) {
            best_score = score;
            best_id = id;
        }
    }
    return BeamSearchResult<Action>{tree.path(best_id), best_score};
}

///  # beam_search<Action, Hash>(state, score, config, expand, apply)
///  undoを書けない場合に使う版。選ばれた候補のぶんだけStateをコピーして持つ。
///  メモリは O(beam_width * sizeof(State))で、1手あたりbeam_width回のコピーが走る。
///  expandとapplyに求めるものはundoあり版と同じなので、
///  undoを書けるようになったらundoを渡すだけで切り替えられる
template <class Action, class Hash, class State, class Expand, class Apply>
BeamSearchResult<Action> beam_search(const State& initial_state, double initial_score, const BeamSearchConfig& config,
                                     Expand expand, Apply apply) {
    assert(config.beam_width > 0);
    SearchTree<Action> tree;
    BeamSelector<Hash> selector;

    std::vector<std::pair<int, double>> frontier{{SearchTree<Action>::ROOT, initial_score}};
    std::vector<State> states{initial_state};
    std::vector<std::pair<int, double>> next_frontier;
    std::vector<State> next_states;

    int best_id = SearchTree<Action>::ROOT;
    double best_score = std::numeric_limits<double>::lowest();

    for (int turn = 0; turn < config.max_turn; turn++) {
        selector.clear();

        for (int i = 0; i < static_cast<int>(frontier.size()); i++) {
            auto [id, score] = frontier[i];
            int produced = 0;
            expand(states[i], [&](const Action& action, double next_score, Hash hash) {
                produced++;
                selector.push(tree.add(id, action), hash, next_score);
            });
            if (produced == 0 && score > best_score) {
                best_score = score;
                best_id = id;
            }
        }

        const auto& selected = selector.select(config.beam_width);
        if (selected.empty()) break;

        //  Stateを実体化するのは選ばれた候補だけ。
        //  親のStateはfrontierの中にあり、frontierはid昇順なので二分探索で引ける
        next_frontier.clear();
        next_states.clear();
        for (const auto& entry: selected) {
            int parent = tree.parent(entry.id);
            auto it = std::lower_bound(frontier.begin(), frontier.end(), parent,
                                       [](const auto& lhs, int value) { return lhs.first < value; });
            assert(it != frontier.end() && it->first == parent);
            next_states.push_back(states[it - frontier.begin()]);
            apply(next_states.back(), tree.action(entry.id));
            next_frontier.push_back({entry.id, entry.score});
        }
        //  次の手でlower_boundを使うためにid昇順へ揃える
        std::vector<int> order(next_frontier.size());
        for (int i = 0; i < static_cast<int>(order.size()); i++) order[i] = i;
        std::sort(order.begin(), order.end(),
                  [&](int lhs, int rhs) { return next_frontier[lhs].first < next_frontier[rhs].first; });
        frontier.clear();
        states.clear();
        for (int i: order) {
            frontier.push_back(next_frontier[i]);
            states.push_back(std::move(next_states[i]));
        }
    }

    for (int i = 0; i < static_cast<int>(frontier.size()); i++) {
        if (frontier[i].second > best_score) {
            best_score = frontier[i].second;
            best_id = frontier[i].first;
        }
    }
    return BeamSearchResult<Action>{tree.path(best_id), best_score};
}

}  //  namespace nono
