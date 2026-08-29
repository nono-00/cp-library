#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

#include "nono/heuristic/search-selector.hpp"
#include "nono/heuristic/search-tree.hpp"
#include "nono/heuristic/timer.hpp"

namespace nono {

///  # ChokudaiSearchConfig
struct ChokudaiSearchConfig {
    ///  最大何手進めるか
    int max_turn;
    ///  1回のパスで各階層から取り出す数
    int chokudai_width;
    ///  ミリ秒。1秒なら1000
    double time_limit;
};

///  # ChokudaiSearchResult
template <class Action>
struct ChokudaiSearchResult {
    std::vector<Action> actions;
    double score;
    ///  何回パスを回せたか
    int passes;
};

///  # chokudai_search<Action, Hash>(state, score, config, expand, apply, undo)
///  ビームサーチが「1階層ずつ、上位widthだけを残して1回で進む」のに対し、
///  chokudaiサーチは「各階層に優先度付きキューを持ち、階層0から順に
///  chokudai_width個ずつ取り出して次の階層へ送る」パスを時間まで繰り返す。
///  探索木も評価も重複排除も共通で、違うのは選択方針と走査順だけ。
///
///  ## 利用者が渡すもの
///  beam_searchと同じ。
///  expand(state, f) : 遷移候補ごとに f(action, score, hash) を呼ぶ。
///                     scoreは遷移後の絶対スコア、hashは遷移後の状態の重複排除キー。
///                     いずれも「次の状態を作らずに」求めること。
///                     一度もfを呼ばなければ、そのノードは終端として扱われる
///  apply(state, action) : stateにactionを適用する
///  undo(state, action)  : applyを打ち消す
///
///  ## undoが必須なこと
///  beam_searchにはStateをコピーして持つundo不要版があるが、こちらには用意していない。
///  ビームサーチが常に2階層ぶんしかノードを抱えないのに対し、chokudaiサーチは
///  抱えるノードが減らないため。重複排除がよく効く問題(状態空間が小さい問題)なら
///  ノード数は飽和するが、状態空間が広くて重複排除が効きにくい問題では
///  パス数に比例して増え続ける。60x60グリッドでの実測では、
///  位置をhashにすると2560パスでも7760ノードで飽和したのに対し、
///  経路ごとに一意なhashにすると1543974ノードまで伸びた。
///  Actionだけならノード1個あたり12バイトなので後者でも18MiB程度で収まるが、
///  Stateを持たせると破綻する
///
///  ## chokudai_widthと状態移動のコスト
///  ビームサーチと違い、取り出すノードは木の中に散らばるので、
///  move_state()の移動距離が大きくなる。60x60グリッド(深さ118)での
///  1取り出しあたりのapply/undo回数の実測:
///    幅1  : 16.0回   幅3  : 40.3回   幅10 : 34.3回   幅100: 23.2回
///  幅1が最も小さいのは、階層t+1で取り出すノードが「直前に作った子」に
///  なりやすいため。apply/undoが重い問題では、この点も幅の選択に効く
template <class Action, class Hash, class State, class Expand, class Apply, class Undo>
ChokudaiSearchResult<Action> chokudai_search(const State& initial_state, double initial_score,
                                             const ChokudaiSearchConfig& config, Expand expand, Apply apply,
                                             Undo undo) {
    assert(config.max_turn > 0);
    assert(config.chokudai_width > 0);

    SearchTree<Action> tree;
    //  階層ごとの優先度付きキュー。最後の階層は取り出すだけなので+1
    std::vector<BestFirstQueue<Hash>> levels(config.max_turn + 1);
    State state = initial_state;
    int current = SearchTree<Action>::ROOT;

    //  階層0には根しか入らないので、hashは何でもよい
    levels[0].push(SearchTree<Action>::ROOT, Hash{}, initial_score);

    int best_id = SearchTree<Action>::ROOT;
    double best_score = std::numeric_limits<double>::lowest();
    bool found_terminal = false;

    Timer timer(config.time_limit);
    //  (node id, score)
    std::vector<std::pair<int, double>> popped;
    int passes = 0;

    while (!timer.expired()) {
        bool progressed = false;
        for (int turn = 0; turn < config.max_turn; turn++) {
            if (timer.expired()) break;
            if (levels[turn].empty()) continue;

            //  先にまとめて取り出してからid順に並べ替える。
            //  取り出しはStateに依存しないので、この並べ替えは結果を変えない。
            //  スコア順のまま辿ると木の中を飛び回ることになり、
            //  move_state()のapply/undoが増える
            popped.clear();
            for (int w = 0; w < config.chokudai_width && !levels[turn].empty(); w++) {
                popped.push_back({levels[turn].top(), levels[turn].top_score()});
                levels[turn].pop();
            }
            std::sort(popped.begin(), popped.end(),
                      [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

            for (auto [id, score]: popped) {
                progressed = true;
                tree.move_state(state, current, id, apply, undo);
                current = id;
                int produced = 0;
                expand(state, [&](const Action& action, double next_score, Hash hash) {
                    produced++;
                    levels[turn + 1].push(tree.add(id, action), hash, next_score);
                });
                //  展開先がないノードは終端。最良のものを答えとする
                if (produced == 0 && (!found_terminal || score > best_score)) {
                    found_terminal = true;
                    best_score = score;
                    best_id = id;
                }
            }
        }
        passes++;
        //  どの階層からも取り出せなくなったら、これ以上パスを回しても何も起きない
        if (!progressed) break;
    }

    //  終端に一度も到達できなかった場合は、到達した中で最良のノードを返す
    if (!found_terminal) {
        for (int turn = config.max_turn; turn >= 0; turn--) {
            if (levels[turn].empty()) continue;
            best_id = levels[turn].top();
            best_score = levels[turn].top_score();
            break;
        }
    }
    return ChokudaiSearchResult<Action>{tree.path(best_id), best_score, passes};
}

}  //  namespace nono
