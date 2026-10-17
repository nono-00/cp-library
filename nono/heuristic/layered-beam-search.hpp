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

//  # LayeredBeamSearchConfig
//  layered_beam_searchに渡す設定。time_limit以降は省略できる
struct LayeredBeamSearchConfig {
    //  最後の層の番号。層は0からmax_layerまであり、層max_layerのノードは展開しない
    int max_layer;
    //  各層で残す候補数の上限。time_limitを指定しなければ、常にこの数まで残す
    int beam_width;
    //  ミリ秒。正の値を指定すると、layered_beam_searchを呼んでからこの時間で終わるように各層で残す候補数を決める
    double time_limit = 0;
    //  time_limitから候補数を決めるときに、何層目まで展開すると見込むか。0ならmax_layer
    int expected_layer = 0;
    //  スコアがこの値以上の候補が選ばれたら、その層で打ち切る
    double goal_score = std::numeric_limits<double>::infinity();
};

//  # LayeredBeamSearchResult
//  layered_beam_searchの結果。答えは、展開先がなかったノード(終端)と最後に残った候補のうち、スコア最良のもの。
//  スコアが同じなら、先に見たものを答えにする。終端は見つかった順(層の順、同じ層では展開した順)に見て、
//  最後に残った候補は展開する順に、すべての終端の後に見る。
//  したがって、同点の終端が複数あれば最も前の層のものが答えになる
template <class Action>
struct LayeredBeamSearchResult {
    //  初期状態から答えのノードまでのAction列
    std::vector<Action> actions;
    //  答えのノードのスコア
    double score;
    //  答えのノードがいる層。1回の遷移で複数層進むので、actions.size()とは一致しない
    int layer;
};

namespace layered_beam_search_internal {

//  答えの候補のうち、スコア最良のもの。同点なら先にupdateしたものを残す
struct Best {
    int id = 0;
    double score = std::numeric_limits<double>::lowest();
    int layer = 0;

    void update(int candidate_id, double candidate_score, int candidate_layer) {
        if (candidate_score > score) {
            id = candidate_id;
            score = candidate_score;
            layer = candidate_layer;
        }
    }
};

//  undoあり版の遷移候補。探索木には選ばれた候補だけを入れる。
//  すべての候補を入れると、木のノード数が「候補数 * 層数」に膨らむため
template <class Action>
struct Candidate {
    //  親のnode id
    int parent;
    double score;
    Action action;
};

//  undoなし版の遷移候補。親を(層, その層のfrontier内の位置)で指し、親のStateを直接引けるようにする
template <class Action>
struct CopyCandidate {
    int parent_layer;
    int parent;
    double score;
    Action action;
};

//  ある層の候補から上位width件を選び、選ばれた候補の番号を昇順にchosenへ入れる。
//  候補の番号は(親の層, 親の層のfrontier内の位置, fを呼んだ順)の辞書順に振られるので、
//  chosenの順に辿ると、親の層の展開順を保ったまま進める。
//  選んだ後の選択器は空のものに置き換える。処理済みの層の候補を抱えたままにすると、
//  全層ぶんの候補が最後までメモリに残るため。
//  goal_score以上の候補が選ばれたかを返す
template <class Hash>
bool select_layer(BeamSelector<Hash>& selector, int width, int candidate_count, double goal_score,
                  std::vector<char>& marked, std::vector<int>& chosen) {
    const auto& selected = selector.select_unordered(width);
    marked.assign(candidate_count, 0);
    bool goal = false;
    for (const auto& entry: selected) {
        marked[entry.id] = 1;
        if (entry.score >= goal_score) goal = true;
    }
    chosen.clear();
    for (int i = 0; i < candidate_count; i++) {
        if (marked[i]) chosen.push_back(i);
    }
    selector = BeamSelector<Hash>();
    return goal;
}

}  //  namespace layered_beam_search_internal

//  # layered_beam_search<Action, Hash>(initial_state, initial_score, config, expand, apply, undo)
//  1回の遷移で複数層先へ進めるビームサーチ。
//  beam_searchが dp[i] から dp[i + 1] にしか配れないのに対し、dp[i] から dp[i + step] へ配れる。
//  層ごとに上位beam_width件を残す。
//  単一のStateを探索木の上で動かす版。
//  applyとundoが互いに逆でないと壊れる。
//  config.max_layer >= 0, config.beam_width > 0 でないと壊れる
//
//  ## 層
//  層の意味は利用者が決める(経過ターン数、置いた個数など)。
//  初期状態は層0にあり、遷移するたびにstepだけ先の層へ進む。
//  候補は同じ層の中でだけスコアを比べられるので、
//  同じ層の状態同士をスコアで比べて意味がある量を層に選ぶ。
//  全ての遷移のstepを1にすると、max_layerをmax_turnとしたbeam_searchと同じ結果を返す
//
//  ## 利用者が渡すもの
//  expand(state, f) : 遷移候補ごとに f(action, score, hash, step) を呼ぶ。
//                     scoreは遷移後の絶対スコア、hashは遷移後の状態の重複排除キー、
//                     stepは何層先へ進むか。step >= 1 でないと壊れる。
//                     scoreとhashは「次の状態を作らずに」求めること。
//                     max_layerを超える遷移は捨てられる。
//                     捨てられなかった遷移が1つもなければ、そのノードは終端として扱われる
//  apply(state, action) : stateにactionを適用する
//  undo(state, action)  : applyを打ち消す
//
//  ## Actionにはundoに必要な情報を持たせる
//  遷移で壊れるが復元が高価な値はActionに退避しておくとundoが軽くなる
//
//  ## 重複排除は層ごと
//  同じhashの候補は、同じ層の中でスコアが最良のもの1件だけを残す。
//  別の層にある同じhashの候補は、別の状態として両方残る。
//  どちらを残すべきかが層の意味とスコアで変わるため
//  (層が消費した資源なら早い層が、進捗なら遅い層が有利になる)
//
//  ## メモリ
//  探索木には各層で選ばれた候補だけを入れる。木は O(beam_width * 層数 * sizeof(Action))。
//  遷移候補は、その層を処理するまで O(候補数 * sizeof(Action)) で持ち、処理したら捨てる
//
//  ## 残す候補数を時間で決める (config.time_limit)
//  beam_searchと同じ(BeamWidthController)。手の代わりに層で数え、
//  各層の展開を終えるたびに、残り時間を残り層数で割った時間に収まるように、次の層で残す候補数を決める。
//  候補が1つも入らない層も1層として数える。
//  残り層数はexpected_layer層目までと見込み、それを超えたらmax_layer層目までとみなす。
//  少なく見込むと、見込みを超えた層では時間が残っておらず、候補数が1になる。
//  幅1では答えが大きく悪くなり、ゴールに着けないこともあるので、多めに見込む
//
//  ## 打ち切り (config.goal_score)
//  ある層でgoal_score以上の候補が選ばれたら、その層を展開せず、その層の候補を最後に残った候補として打ち切る。
//  層は前から順に処理するので、goal_scoreに達する最も前の層で止まる。
//  初期状態のスコアがgoal_score以上なら、何も展開せずに初期状態を答えにする
//
//  ## 制限
//  - 各層のノードは候補が作られた順に辿る。層と木の深さがずれるのでこの順はDFS順にならず、
//    apply/undoの回数はDFS順で辿る場合より増える
//  - 選択器をmax_layer + 1個、最初に確保する。
//    max_layerが大きいと、確保だけで時間とメモリを使う
template <class Action, class Hash, class State, class Expand, class Apply, class Undo>
LayeredBeamSearchResult<Action> layered_beam_search(const State& initial_state, double initial_score,
                                                    const LayeredBeamSearchConfig& config, Expand expand, Apply apply,
                                                    Undo undo) {
    assert(config.max_layer >= 0);
    assert(config.beam_width > 0);
    //  選択器の確保も制限時間に含めるため、最初に作る
    BeamWidthController width(config.beam_width, config.time_limit, config.expected_layer, config.max_layer);
    SearchTree<Action> tree;
    //  層ごとの選択器と遷移候補。層layerのノードを展開すると、層layer + stepに候補が入る。
    //  選択器に渡すidは、その層のcandidates内の位置
    std::vector<BeamSelector<Hash>> selectors(config.max_layer + 1);
    std::vector<std::vector<layered_beam_search_internal::Candidate<Action>>> candidates(config.max_layer + 1);
    State state = initial_state;
    int current = SearchTree<Action>::ROOT;

    //  (node id, score)
    std::vector<std::pair<int, double>> frontier{{SearchTree<Action>::ROOT, initial_score}};
    std::vector<char> marked;
    std::vector<int> chosen;
    layered_beam_search_internal::Best best;
    bool goal = initial_score >= config.goal_score;
    //  直前の層で展開したノード数
    int expanded = 0;

    for (int layer = 0; layer <= config.max_layer; layer++) {
        if (layer > 0) {
            //  この層に残す候補を選び、選ばれたものだけを木に入れる
            auto& layer_candidates = candidates[layer];
            goal = layered_beam_search_internal::select_layer(selectors[layer], width.next(layer - 1, expanded),
                                                              static_cast<int>(layer_candidates.size()),
                                                              config.goal_score, marked, chosen);
            frontier.clear();
            for (int c: chosen) {
                const auto& candidate = layer_candidates[c];
                frontier.push_back({tree.add(candidate.parent, candidate.action), candidate.score});
            }
            layer_candidates = std::vector<layered_beam_search_internal::Candidate<Action>>();
        }
        //  最後の層と、goal_scoreに達した層は展開しない。残った候補はそのまま答えの候補
        if (layer == config.max_layer || goal) {
            for (auto [id, score]: frontier) best.update(id, score, layer);
            break;
        }
        //  候補が1つも入らなかった層は何もせず次へ進む。
        //  より先の層には候補が入っていることがあるので、ここで打ち切ってはいけない
        for (auto [id, score]: frontier) {
            tree.move_state(state, current, id, apply, undo);
            current = id;
            int produced = 0;
            expand(state, [&](const Action& action, double next_score, Hash hash, int step) {
                assert(step >= 1);
                //  max_layerを超える遷移は捨てる
                if (step > config.max_layer - layer) return;
                produced++;
                auto& target = candidates[layer + step];
                selectors[layer + step].push(static_cast<int>(target.size()), hash, next_score);
                target.push_back({id, next_score, action});
            });
            //  展開先がないノードは終端。答えの候補として覚えておく
            if (produced == 0) best.update(id, score, layer);
        }
        expanded = static_cast<int>(frontier.size());
    }
    return LayeredBeamSearchResult<Action>{tree.path(best.id), best.score, best.layer};
}

//  # layered_beam_search<Action, Hash>(initial_state, initial_score, config, expand, apply)
//  undoを書けない場合に使う版。選ばれた候補のぶんだけStateをコピーして持つ。
//  ある層のStateは、そこから配った最も遠い層のStateを作り終えるまで持ち続けるので、
//  Stateのメモリは O(beam_width * stepの最大値 * sizeof(State))。
//  expandとapplyに求めるもの、configの意味、返す結果はundoあり版と同じなので、
//  undoを書けるようになったらundoを渡すだけで切り替えられる
//  config.max_layer >= 0, config.beam_width > 0 でないと壊れる
template <class Action, class Hash, class State, class Expand, class Apply>
LayeredBeamSearchResult<Action> layered_beam_search(const State& initial_state, double initial_score,
                                                    const LayeredBeamSearchConfig& config, Expand expand, Apply apply) {
    assert(config.max_layer >= 0);
    assert(config.beam_width > 0);
    BeamWidthController width(config.beam_width, config.time_limit, config.expected_layer, config.max_layer);
    SearchTree<Action> tree;
    std::vector<BeamSelector<Hash>> selectors(config.max_layer + 1);
    std::vector<std::vector<layered_beam_search_internal::CopyCandidate<Action>>> candidates(config.max_layer + 1);

    //  層ごとの(node id, score)とState。同じ添字が同じノードを指す
    std::vector<std::vector<std::pair<int, double>>> frontiers(config.max_layer + 1);
    std::vector<std::vector<State>> states(config.max_layer + 1);
    frontiers[0].push_back({SearchTree<Action>::ROOT, initial_score});
    states[0].push_back(initial_state);
    //  release_after[layer]: 層layerのStateを作り終えたら、Stateを捨ててよい層
    std::vector<std::vector<int>> release_after(config.max_layer + 1);

    auto release = [&](int layer) {
        frontiers[layer] = std::vector<std::pair<int, double>>();
        states[layer] = std::vector<State>();
    };

    std::vector<char> marked;
    std::vector<int> chosen;
    layered_beam_search_internal::Best best;
    bool goal = initial_score >= config.goal_score;
    int expanded = 0;

    for (int layer = 0; layer <= config.max_layer; layer++) {
        auto& frontier = frontiers[layer];
        if (layer > 0) {
            auto& layer_candidates = candidates[layer];
            goal = layered_beam_search_internal::select_layer(selectors[layer], width.next(layer - 1, expanded),
                                                              static_cast<int>(layer_candidates.size()),
                                                              config.goal_score, marked, chosen);
            //  Stateを実体化するのは選ばれた候補だけ。親のStateは候補が指す位置で引ける
            for (int c: chosen) {
                const auto& candidate = layer_candidates[c];
                states[layer].push_back(states[candidate.parent_layer][candidate.parent]);
                apply(states[layer].back(), candidate.action);
                int parent_id = frontiers[candidate.parent_layer][candidate.parent].first;
                frontier.push_back({tree.add(parent_id, candidate.action), candidate.score});
            }
            layer_candidates = std::vector<layered_beam_search_internal::CopyCandidate<Action>>();
            for (int source: release_after[layer]) release(source);
        }
        if (layer == config.max_layer || goal) {
            for (auto [id, score]: frontier) best.update(id, score, layer);
            break;
        }

        //  この層から配った最も遠い層
        int farthest = layer;
        for (int i = 0; i < static_cast<int>(frontier.size()); i++) {
            auto [id, score] = frontier[i];
            int produced = 0;
            expand(states[layer][i], [&](const Action& action, double next_score, Hash hash, int step) {
                assert(step >= 1);
                if (step > config.max_layer - layer) return;
                produced++;
                farthest = std::max(farthest, layer + step);
                auto& target = candidates[layer + step];
                selectors[layer + step].push(static_cast<int>(target.size()), hash, next_score);
                target.push_back({layer, i, next_score, action});
            });
            if (produced == 0) best.update(id, score, layer);
        }
        expanded = static_cast<int>(frontier.size());

        //  この層のStateを親として使うのは、層farthestのStateを作るときが最後
        if (farthest == layer) {
            release(layer);
        } else {
            release_after[farthest].push_back(layer);
        }
    }
    return LayeredBeamSearchResult<Action>{tree.path(best.id), best.score, best.layer};
}

}  //  namespace nono
