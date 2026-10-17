#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

#include "./search-selector.hpp"
#include "./search-tree.hpp"

namespace nono {

//  # LayeredBeamSearchConfig
//  layered_beam_searchに渡す設定
struct LayeredBeamSearchConfig {
    //  最後の層の番号。層は0からmax_layerまであり、層max_layerのノードは展開しない
    int max_layer;
    //  各層で残す候補数
    int beam_width;
};

//  # LayeredBeamSearchResult
//  layered_beam_searchの結果。答えは、展開先がなかったノード(終端)と
//  層max_layerに残った候補のうち、スコア最良のもの
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

//  答えの候補のうち、スコア最良のもの
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

//  選択器から上位width件を(node id, score)としてスコア降順で取り出す。
//  取り出した後の選択器は空のものに置き換える。処理済みの層の候補を抱えたままにすると、
//  全層ぶんの候補が最後までメモリに残るため
template <class Hash>
void take_frontier(BeamSelector<Hash>& selector, int width, std::vector<std::pair<int, double>>& frontier) {
    frontier.clear();
    for (const auto& entry: selector.select(width)) frontier.push_back({entry.id, entry.score});
    selector = BeamSelector<Hash>();
}

template <class Frontier>
void sort_by_id(Frontier& frontier) {
    std::sort(frontier.begin(), frontier.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
}

}  //  namespace layered_beam_search_internal

//  # layered_beam_search<Action, Hash>(initial_state, initial_score, config, expand, apply, undo)
//  1回の遷移で複数層先へ進めるビームサーチ。
//  beam_searchが dp[i] から dp[i + 1] にしか配れないのに対し、dp[i] から dp[i + step] へ配れる。
//  層ごとに上位beam_width件を残す。
//  単一のStateを探索木の上で動かす版。ノードはActionしか持たないので、
//  メモリは O(生成ノード数 * sizeof(Action))。
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
//  ## 制限
//  - 各層のノードはid順に辿る。層と木の深さがずれるのでid順はDFS順にならず、
//    apply/undoの回数はDFS順で辿る場合より増える
//  - 選択器をmax_layer + 1個、最初に確保する。
//    max_layerが大きいと、確保だけで時間とメモリを使う
template <class Action, class Hash, class State, class Expand, class Apply, class Undo>
LayeredBeamSearchResult<Action> layered_beam_search(const State& initial_state, double initial_score,
                                                    const LayeredBeamSearchConfig& config, Expand expand, Apply apply,
                                                    Undo undo) {
    assert(config.max_layer >= 0);
    assert(config.beam_width > 0);
    SearchTree<Action> tree;
    //  層ごとの選択器。層layerのノードを展開すると、層layer + stepの選択器に候補が入る
    std::vector<BeamSelector<Hash>> selectors(config.max_layer + 1);
    State state = initial_state;
    int current = SearchTree<Action>::ROOT;

    //  層0には根しか入らないので、hashは何でもよい
    selectors[0].push(SearchTree<Action>::ROOT, Hash{}, initial_score);

    //  (node id, score)
    std::vector<std::pair<int, double>> frontier;
    layered_beam_search_internal::Best best;

    for (int layer = 0; layer <= config.max_layer; layer++) {
        layered_beam_search_internal::take_frontier(selectors[layer], config.beam_width, frontier);
        //  最後の層は展開しない。残った候補はそのまま答えの候補
        if (layer == config.max_layer) {
            for (auto [id, score]: frontier) best.update(id, score, layer);
            break;
        }
        //  候補が1つも入らなかった層は何もせず次へ進む。
        //  より先の層には候補が入っていることがあるので、ここで打ち切ってはいけない
        layered_beam_search_internal::sort_by_id(frontier);

        for (auto [id, score]: frontier) {
            tree.move_state(state, current, id, apply, undo);
            current = id;
            int produced = 0;
            expand(state, [&](const Action& action, double next_score, Hash hash, int step) {
                assert(step >= 1);
                //  max_layerを超える遷移は捨てる
                if (step > config.max_layer - layer) return;
                produced++;
                selectors[layer + step].push(tree.add(id, action), hash, next_score);
            });
            //  展開先がないノードは終端。答えの候補として覚えておく
            if (produced == 0) best.update(id, score, layer);
        }
    }
    return LayeredBeamSearchResult<Action>{tree.path(best.id), best.score, best.layer};
}

//  # layered_beam_search<Action, Hash>(initial_state, initial_score, config, expand, apply)
//  undoを書けない場合に使う版。選ばれた候補のぶんだけStateをコピーして持つ。
//  ある層のStateは、そこから配った最も遠い層のStateを作り終えるまで持ち続けるので、
//  メモリは O(beam_width * stepの最大値 * sizeof(State))。
//  expandとapplyに求めるものと返す結果はundoあり版と同じなので、
//  undoを書けるようになったらundoを渡すだけで切り替えられる
//  config.max_layer >= 0, config.beam_width > 0 でないと壊れる
template <class Action, class Hash, class State, class Expand, class Apply>
LayeredBeamSearchResult<Action> layered_beam_search(const State& initial_state, double initial_score,
                                                    const LayeredBeamSearchConfig& config, Expand expand, Apply apply) {
    assert(config.max_layer >= 0);
    assert(config.beam_width > 0);
    SearchTree<Action> tree;
    std::vector<BeamSelector<Hash>> selectors(config.max_layer + 1);
    selectors[0].push(SearchTree<Action>::ROOT, Hash{}, initial_score);

    //  層ごとの(node id, score)とState。どちらもid昇順で、同じ添字が同じノードを指す
    std::vector<std::vector<std::pair<int, double>>> frontiers(config.max_layer + 1);
    std::vector<std::vector<State>> states(config.max_layer + 1);
    //  first_child[layer]: 層layerの展開を始めた時点のノード数。
    //  子は親の展開中に作られるので、層layerのノードの子のidは
    //  first_child[layer]以上、first_child[layer + 1]未満になる
    std::vector<int> first_child(config.max_layer + 1);
    //  release_after[layer]: 層layerのStateを作り終えたら、Stateを捨ててよい層
    std::vector<std::vector<int>> release_after(config.max_layer + 1);

    auto release = [&](int layer) {
        frontiers[layer] = std::vector<std::pair<int, double>>();
        states[layer] = std::vector<State>();
    };

    layered_beam_search_internal::Best best;

    for (int layer = 0; layer <= config.max_layer; layer++) {
        auto& frontier = frontiers[layer];
        layered_beam_search_internal::take_frontier(selectors[layer], config.beam_width, frontier);
        if (layer == config.max_layer) {
            for (auto [id, score]: frontier) best.update(id, score, layer);
            break;
        }
        //  後の層から二分探索で引けるようにid昇順へ揃える
        layered_beam_search_internal::sort_by_id(frontier);

        //  Stateを実体化するのは選ばれた候補だけ
        for (const auto& node: frontier) {
            int id = node.first;
            if (id == SearchTree<Action>::ROOT) {
                states[layer].push_back(initial_state);
                continue;
            }
            //  親がいるのは、first_childがid以下である最後の層
            auto first = first_child.begin();
            int parent_layer = static_cast<int>(std::upper_bound(first, first + layer, id) - first) - 1;
            assert(0 <= parent_layer && parent_layer < layer);
            const auto& parent_frontier = frontiers[parent_layer];
            int parent = tree.parent(id);
            auto it = std::lower_bound(parent_frontier.begin(), parent_frontier.end(), parent,
                                       [](const auto& lhs, int value) { return lhs.first < value; });
            assert(it != parent_frontier.end() && it->first == parent);
            states[layer].push_back(states[parent_layer][it - parent_frontier.begin()]);
            apply(states[layer].back(), tree.action(id));
        }
        for (int source: release_after[layer]) release(source);

        first_child[layer] = tree.size();
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
                selectors[layer + step].push(tree.add(id, action), hash, next_score);
            });
            if (produced == 0) best.update(id, score, layer);
        }

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
