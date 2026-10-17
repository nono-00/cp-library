#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

#include "./beam-width.hpp"
#include "./search-selector.hpp"
#include "./search-tree.hpp"

namespace nono {

//  # BeamSearchConfig
//  beam_searchに渡す設定。time_limit以降は省略できる
struct BeamSearchConfig {
    //  最大何手進めるか
    int max_turn;
    //  各手で残す候補数の上限。time_limitを指定しなければ、常にこの数まで残す
    int beam_width;
    //  ミリ秒。正の値を指定すると、beam_searchを呼んでからこの時間で終わるように各手で残す候補数を決める
    double time_limit = 0;
    //  time_limitから候補数を決めるときに見込む手数。0ならmax_turn
    int expected_turn = 0;
    //  スコアがこの値以上の候補が出たら、その手で打ち切る
    double goal_score = std::numeric_limits<double>::infinity();
};

//  # BeamSearchResult
//  beam_searchの結果。答えは、展開先がなかったノード(終端)と最後に残った候補のうち、スコア最良のもの。
//  スコアが同じなら、先に見たものを答えにする。終端は見つかった順(浅い順、同じ深さではDFS順)に見て、
//  最後に残った候補はDFS順に、すべての終端の後に見る。
//  したがって、同点の終端が複数あれば最も浅いものが答えになる
template <class Action>
struct BeamSearchResult {
    //  初期状態から答えのノードまでのAction列
    std::vector<Action> actions;
    //  答えのノードのスコア
    double score;
};

namespace beam_search_internal {

//  1手ぶんの遷移候補。探索木には選ばれた候補だけを入れる。
//  すべての候補を入れると、木のノード数が「候補数 * 手数」に膨らむため
template <class Action>
struct Candidate {
    //  親のfrontier内の位置
    int parent;
    double score;
    Action action;
};

//  答えの候補のうち、スコア最良のもの。同点なら先にupdateしたものを残す
struct Best {
    int id = 0;
    double score = std::numeric_limits<double>::lowest();

    void update(int candidate_id, double candidate_score) {
        if (candidate_score > score) {
            id = candidate_id;
            score = candidate_score;
        }
    }
};

//  選ばれた候補の番号を昇順にchosenへ入れる。
//  候補の番号は(親のfrontier内の位置, fを呼んだ順)の辞書順に振られるので、
//  frontierがDFS順なら、chosenの順に木へ入れた候補もDFS順になる。
//  選ばれた番号を並べ替える O(width log width) の代わりに、印を付けて全候補を走査する O(候補数) にしている。
//  候補数が幅の数倍程度に収まる典型的な場合は、こちらの方が速い
template <class Entry>
void collect_chosen(const std::vector<Entry>& selected, int candidate_count, std::vector<char>& marked,
                    std::vector<int>& chosen) {
    marked.assign(candidate_count, 0);
    for (const auto& entry: selected) marked[entry.id] = 1;
    chosen.clear();
    for (int i = 0; i < candidate_count; i++) {
        if (marked[i]) chosen.push_back(i);
    }
}

//  選ばれた候補にgoal_score以上のものがあるか
template <class Entry>
bool reached_goal(const std::vector<Entry>& selected, double goal_score) {
    return std::any_of(selected.begin(), selected.end(), [&](const Entry& entry) { return entry.score >= goal_score; });
}

}  //  namespace beam_search_internal

//  # beam_search<Action, Hash>(initial_state, initial_score, config, expand, apply, undo)
//  単一のStateを探索木の上で動かす版。
//  frontierのノードをDFS順に辿るので、1手あたりのapply/undoの回数は、
//  根とfrontierのノードを結ぶ部分木の辺数の2倍以下に収まる。
//  applyとundoが互いに逆でないと壊れる。config.beam_width > 0 でないと壊れる
//
//  ## 利用者が渡すもの
//  expand(state, f) : 遷移候補ごとに f(action, score, hash) を呼ぶ。
//                     scoreは遷移後の絶対スコア、hashは遷移後の状態の重複排除キー。
//                     いずれも「次の状態を作らずに」求めること。
//                     一度もfを呼ばなければ、そのノードは終端として扱われる
//  apply(state, action) : stateにactionを適用する
//  undo(state, action)  : applyを打ち消す
//
//  ## Actionにはundoに必要な情報を持たせる
//  遷移で壊れるが復元が高価な値はActionに退避しておくとundoが軽くなる
//
//  ## メモリ
//  探索木には各手で選ばれた候補だけを入れる。
//  木は O(beam_width * 手数 * sizeof(Action))、1手ぶんの候補は O(候補数 * sizeof(Action))
//
//  ## 残す候補数を時間で決める (config.time_limit)
//  各手の展開を終えるたびに、残り時間を残り手数で割った時間に収まるように、次の手で残す候補数を決める
//  (BeamWidthController)。
//  候補数はbeam_widthを超えず、1を下回らない。時間を使い切った後は1つずつ残して最後まで進む。
//  残り手数はexpected_turn手目までと見込み、それを超えたらmax_turn手目までとみなす。
//  ゴールに着く手数がケースごとに違う問題では、expected_turnを多めに見込む。
//  少なく見込むと、見込みを超えた手では時間が残っておらず、候補数が1になる。
//  幅1では答えが大きく悪くなり、ゴールに着けないこともある。
//  多く見込むと、早くゴールに着いた分の時間が余る
//
//  ## 打ち切り (config.goal_score)
//  ある手でgoal_score以上の候補が出たら、その手で選んだ候補を最後に残った候補として打ち切る。
//  初期状態のスコアがgoal_score以上なら、何も展開せずに初期状態を答えにする。
//  これ以上のスコアは出ないと分かっているとき(ゴールに着いたら決まった値になる場合など)に使う。
//  答えのスコアがgoal_score未満なら、ゴールに着けなかったと分かる
template <class Action, class Hash, class State, class Expand, class Apply, class Undo>
BeamSearchResult<Action> beam_search(const State& initial_state, double initial_score, const BeamSearchConfig& config,
                                     Expand expand, Apply apply, Undo undo) {
    assert(config.beam_width > 0);
    BeamWidthController width(config.beam_width, config.time_limit, config.expected_turn, config.max_turn);
    SearchTree<Action> tree;
    BeamSelector<Hash> selector;
    State state = initial_state;
    int current = SearchTree<Action>::ROOT;

    //  (node id, score)。DFS順に並ぶ
    std::vector<std::pair<int, double>> frontier{{SearchTree<Action>::ROOT, initial_score}};
    std::vector<std::pair<int, double>> next_frontier;
    std::vector<beam_search_internal::Candidate<Action>> candidates;
    std::vector<char> marked;
    std::vector<int> chosen;
    beam_search_internal::Best best;
    bool goal = initial_score >= config.goal_score;

    for (int turn = 0; turn < config.max_turn && !goal; turn++) {
        selector.clear();
        candidates.clear();

        for (int i = 0; i < static_cast<int>(frontier.size()); i++) {
            auto [id, score] = frontier[i];
            tree.move_state(state, current, id, apply, undo);
            current = id;
            int produced = 0;
            expand(state, [&](const Action& action, double next_score, Hash hash) {
                produced++;
                selector.push(static_cast<int>(candidates.size()), hash, next_score);
                candidates.push_back({i, next_score, action});
            });
            //  展開先がないノードは終端。答えの候補として覚えておく
            if (produced == 0) best.update(id, score);
        }

        const auto& selected = selector.select_unordered(width.next(turn, static_cast<int>(frontier.size())));
        if (selected.empty()) break;
        goal = beam_search_internal::reached_goal(selected, config.goal_score);
        beam_search_internal::collect_chosen(selected, static_cast<int>(candidates.size()), marked, chosen);

        next_frontier.clear();
        for (int c: chosen) {
            const auto& candidate = candidates[c];
            next_frontier.push_back({tree.add(frontier[candidate.parent].first, candidate.action), candidate.score});
        }
        std::swap(frontier, next_frontier);
    }

    //  最後に残った候補も答えの候補
    for (auto [id, score]: frontier) best.update(id, score);
    return BeamSearchResult<Action>{tree.path(best.id), best.score};
}

//  # beam_search<Action, Hash>(initial_state, initial_score, config, expand, apply)
//  undoを書けない場合に使う版。選ばれた候補のぶんだけStateをコピーして持つ。
//  Stateのメモリは O(beam_width * sizeof(State))で、1手あたり選ばれた候補の数だけコピーが走る。
//  expandとapplyに求めるもの、configの意味、返す結果はundoあり版と同じなので、
//  undoを書けるようになったらundoを渡すだけで切り替えられる
//  config.beam_width > 0 でないと壊れる
template <class Action, class Hash, class State, class Expand, class Apply>
BeamSearchResult<Action> beam_search(const State& initial_state, double initial_score, const BeamSearchConfig& config,
                                     Expand expand, Apply apply) {
    assert(config.beam_width > 0);
    BeamWidthController width(config.beam_width, config.time_limit, config.expected_turn, config.max_turn);
    SearchTree<Action> tree;
    BeamSelector<Hash> selector;

    //  frontier[i] = (node id, score)。そのStateはstates[i]
    std::vector<std::pair<int, double>> frontier{{SearchTree<Action>::ROOT, initial_score}};
    std::vector<State> states{initial_state};
    std::vector<std::pair<int, double>> next_frontier;
    std::vector<State> next_states;
    std::vector<beam_search_internal::Candidate<Action>> candidates;
    std::vector<char> marked;
    std::vector<int> chosen;
    beam_search_internal::Best best;
    bool goal = initial_score >= config.goal_score;

    for (int turn = 0; turn < config.max_turn && !goal; turn++) {
        selector.clear();
        candidates.clear();

        for (int i = 0; i < static_cast<int>(frontier.size()); i++) {
            int produced = 0;
            expand(states[i], [&](const Action& action, double next_score, Hash hash) {
                produced++;
                selector.push(static_cast<int>(candidates.size()), hash, next_score);
                candidates.push_back({i, next_score, action});
            });
            if (produced == 0) best.update(frontier[i].first, frontier[i].second);
        }

        const auto& selected = selector.select_unordered(width.next(turn, static_cast<int>(frontier.size())));
        if (selected.empty()) break;
        goal = beam_search_internal::reached_goal(selected, config.goal_score);
        beam_search_internal::collect_chosen(selected, static_cast<int>(candidates.size()), marked, chosen);

        //  Stateを実体化するのは選ばれた候補だけ。親のStateは候補が持つ位置で引ける
        next_frontier.clear();
        next_states.clear();
        for (int c: chosen) {
            const auto& candidate = candidates[c];
            next_states.push_back(states[candidate.parent]);
            apply(next_states.back(), candidate.action);
            next_frontier.push_back({tree.add(frontier[candidate.parent].first, candidate.action), candidate.score});
        }
        std::swap(frontier, next_frontier);
        std::swap(states, next_states);
    }

    for (auto [id, score]: frontier) best.update(id, score);
    return BeamSearchResult<Action>{tree.path(best.id), best.score};
}

}  //  namespace nono
