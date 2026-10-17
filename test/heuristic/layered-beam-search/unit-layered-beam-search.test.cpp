// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <vector>

#include "nono/heuristic/beam-search.hpp"
#include "nono/heuristic/layered-beam-search.hpp"

namespace nono {

using Hash = unsigned long long;

constexpr double UNREACHABLE = -1e18;

//  ---------------- グリッド経路最大化 ----------------
//  (0,0)から右/下に進み、着いたマスの値の総和を最大化する。
//  1回の遷移で1〜max_lengthマス進める。層は「行 + 列」で、進んだマス数がそのままstepになる。
//  DPで厳密解が求まるので、ビーム幅を十分大きくすれば一致する

namespace grid {

constexpr int H = 14, W = 14;
double value[H][W];

struct Action {
    int direction;  //  0:右 1:下
    int length;     //  進むマス数
    bool operator==(const Action&) const = default;
};

struct State {
    int row, column;
    double score;
};

void initialize(unsigned seed) {
    std::mt19937 mt(seed);
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) value[i][j] = (mt() % 1000) + 0.5;
    }
}

Hash hash_of(int row, int column) {
    return static_cast<Hash>(row) * W + column;
}

void apply(State& state, const Action& action) {
    if (action.direction == 0) {
        state.column += action.length;
    } else {
        state.row += action.length;
    }
    state.score += value[state.row][state.column];
}

void undo(State& state, const Action& action) {
    state.score -= value[state.row][state.column];
    if (action.direction == 0) {
        state.column -= action.length;
    } else {
        state.row -= action.length;
    }
}

//  次の状態を作らずにscoreとhashを渡す
template <class F>
void expand(const State& state, int max_length, F f) {
    for (int length = 1; length <= max_length; length++) {
        if (state.column + length < W) {
            f(Action{0, length}, state.score + value[state.row][state.column + length],
              hash_of(state.row, state.column + length), length);
        }
        if (state.row + length < H) {
            f(Action{1, length}, state.score + value[state.row + length][state.column],
              hash_of(state.row + length, state.column), length);
        }
    }
}

//  層(行 + 列)がlayerであるマスに着く経路の最大スコア
double optimum(int max_length, int layer) {
    static double dp[H][W];
    double best_in_layer = UNREACHABLE;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            double best = UNREACHABLE;
            if (i == 0 && j == 0) best = 0;
            for (int length = 1; length <= max_length; length++) {
                if (i >= length) best = std::max(best, dp[i - length][j]);
                if (j >= length) best = std::max(best, dp[i][j - length]);
            }
            dp[i][j] = value[i][j] + best;
            if (i + j == layer) best_in_layer = std::max(best_in_layer, dp[i][j]);
        }
    }
    return best_in_layer;
}

//  返ってきたActionの列を実際に辿り直した状態
State replay(const std::vector<Action>& actions) {
    State state{0, 0, value[0][0]};
    for (const Action& action: actions) apply(state, action);
    return state;
}

}  //  namespace grid

//  全ての遷移のstepが1なら、beam_searchと完全に同じ答えを返すこと
void test_same_as_beam_search(unsigned seed, int width, int max_turn) {
    grid::initialize(seed);
    grid::State initial{0, 0, grid::value[0][0]};
    auto expand_with_step = [](const grid::State& s, auto f) {
        grid::expand(s, 1, f);
    };
    auto expand_without_step = [](const grid::State& s, auto f) {
        grid::expand(s, 1, [&](const grid::Action& action, double score, Hash hash, int) { f(action, score, hash); });
    };

    auto expected = beam_search<grid::Action, Hash>(initial, initial.score, BeamSearchConfig{max_turn, width},
                                                    expand_without_step, grid::apply, grid::undo);
    LayeredBeamSearchConfig config{max_turn, width};
    auto with_undo = layered_beam_search<grid::Action, Hash>(initial, initial.score, config, expand_with_step,
                                                             grid::apply, grid::undo);
    auto without_undo =
        layered_beam_search<grid::Action, Hash>(initial, initial.score, config, expand_with_step, grid::apply);

    assert(with_undo.actions == expected.actions);
    assert(with_undo.score == expected.score);
    assert(without_undo.actions == expected.actions);
    assert(without_undo.score == expected.score);
    //  1層ずつ進むので、層は手数と一致する
    assert(with_undo.layer == static_cast<int>(expected.actions.size()));
    assert(without_undo.layer == with_undo.layer);
}

//  1回の遷移で1マスか2マス進める。層を飛び越す遷移が混ざる
void test_jump_grid(unsigned seed, int width, int max_layer, bool expect_optimal) {
    constexpr int MAX_LENGTH = 2;
    grid::initialize(seed);
    grid::State initial{0, 0, grid::value[0][0]};
    LayeredBeamSearchConfig config{max_layer, width};
    auto expand = [](const grid::State& s, auto f) {
        grid::expand(s, MAX_LENGTH, f);
    };

    auto with_undo =
        layered_beam_search<grid::Action, Hash>(initial, initial.score, config, expand, grid::apply, grid::undo);
    auto without_undo = layered_beam_search<grid::Action, Hash>(initial, initial.score, config, expand, grid::apply);

    //  2つの版は完全に同じ答えを返すこと
    assert(with_undo.actions == without_undo.actions);
    assert(with_undo.score == without_undo.score);
    assert(with_undo.layer == without_undo.layer);

    //  報告されたスコアと層が、実際に辿り直したものと一致すること
    grid::State final_state = grid::replay(with_undo.actions);
    assert(std::abs(final_state.score - with_undo.score) < 1e-6);
    assert(final_state.row + final_state.column == with_undo.layer);

    //  右下の角以外では1マス進む遷移が必ず残るので、答えは最後の層か角にある
    int last_layer = std::min(max_layer, grid::H + grid::W - 2);
    assert(with_undo.layer == last_layer);

    double optimum = grid::optimum(MAX_LENGTH, last_layer);
    assert(with_undo.score <= optimum + 1e-6);
    if (expect_optimal) assert(with_undo.score > optimum - 1e-6);
}

//  max_layerが0なら、根がそのまま返ること
void test_root_only() {
    grid::initialize(1);
    grid::State initial{0, 0, grid::value[0][0]};
    LayeredBeamSearchConfig config{0, 10};
    auto expand = [](const grid::State& s, auto f) {
        grid::expand(s, 2, f);
    };

    auto with_undo =
        layered_beam_search<grid::Action, Hash>(initial, initial.score, config, expand, grid::apply, grid::undo);
    auto without_undo = layered_beam_search<grid::Action, Hash>(initial, initial.score, config, expand, grid::apply);

    assert(with_undo.actions.empty() && with_undo.score == initial.score && with_undo.layer == 0);
    assert(without_undo.actions.empty() && without_undo.score == initial.score && without_undo.layer == 0);
}

//  ---------------- 1次元のカエル ----------------
//  位置0から始め、位置ごとに決まった幅(1〜3の部分集合)だけ前へ跳ぶ。着地した位置の得点の総和を最大化する。
//  層は位置。goalを超える跳躍は探索側で捨てられるので、expandでは範囲を確かめない。
//  跳べなくなった位置が終端になる

namespace frog {

int goal;
//  reward[p]: 位置pに着地したときの得点。goalを超える跳躍でも読めるように長めに持つ
std::vector<double> reward;
//  jumps[p]: 位置pから跳べる幅
std::vector<std::vector<int>> jumps;

struct Action {
    int distance;
    bool operator==(const Action&) const = default;
};

struct State {
    int position;
    double score;
};

void initialize(unsigned seed, int new_goal) {
    std::mt19937 mt(seed);
    goal = new_goal;
    reward.assign(goal + 4, 0);
    for (double& r: reward) r = (mt() % 1000) + 0.5;
    jumps.assign(goal + 1, {});
    for (int p = 0; p <= goal; p++) {
        for (int distance = 1; distance <= 3; distance++) {
            if (mt() % 4 != 0) jumps[p].push_back(distance);
        }
    }
}

void apply(State& state, const Action& action) {
    state.position += action.distance;
    state.score += reward[state.position];
}

void undo(State& state, const Action& action) {
    state.score -= reward[state.position];
    state.position -= action.distance;
}

template <class F>
void expand(const State& state, F f) {
    for (int distance: jumps[state.position]) {
        int next = state.position + distance;
        f(Action{distance}, state.score + reward[next], static_cast<Hash>(next), distance);
    }
}

struct Summary {
    //  終端(範囲内の跳躍が残っていない位置)のうち最大のスコア
    double optimum;
    //  到達できない位置があるか。あれば、候補が1つも入らない層ができる
    bool has_empty_layer;
    //  到達できる位置から、goalを超える跳躍があるか
    bool drops_transition;
};

Summary summarize() {
    std::vector<double> dp(goal + 1, UNREACHABLE);
    dp[0] = 0;
    Summary summary{UNREACHABLE, false, false};
    for (int p = 0; p <= goal; p++) {
        if (dp[p] == UNREACHABLE) {
            summary.has_empty_layer = true;
            continue;
        }
        bool moved = false;
        for (int distance: jumps[p]) {
            if (p + distance > goal) {
                summary.drops_transition = true;
                continue;
            }
            moved = true;
            dp[p + distance] = std::max(dp[p + distance], dp[p] + reward[p + distance]);
        }
        if (!moved) summary.optimum = std::max(summary.optimum, dp[p]);
    }
    return summary;
}

State replay(const std::vector<Action>& actions) {
    State state{0, 0};
    for (const Action& action: actions) apply(state, action);
    return state;
}

}  //  namespace frog

//  test_frogが通した経路の数。狙った経路を1回も通らないままテストが通ることを防ぐ
struct FrogCoverage {
    int empty_layer = 0;
    int dropped_transition = 0;
    int answer_before_goal = 0;
    int answer_at_goal = 0;
};

void test_frog(unsigned seed, int goal, int width, FrogCoverage& coverage) {
    frog::initialize(seed, goal);
    frog::State initial{0, 0};
    LayeredBeamSearchConfig config{goal, width};
    auto expand = [](const frog::State& s, auto f) {
        frog::expand(s, f);
    };

    auto with_undo = layered_beam_search<frog::Action, Hash>(initial, 0.0, config, expand, frog::apply, frog::undo);
    auto without_undo = layered_beam_search<frog::Action, Hash>(initial, 0.0, config, expand, frog::apply);

    assert(with_undo.actions == without_undo.actions);
    assert(with_undo.score == without_undo.score);
    assert(with_undo.layer == without_undo.layer);

    frog::State final_state = frog::replay(with_undo.actions);
    assert(std::abs(final_state.score - with_undo.score) < 1e-6);
    assert(final_state.position == with_undo.layer);
    assert(with_undo.layer <= goal);
    //  答えの位置からは、範囲内の跳躍が残っていないこと
    for (int distance: frog::jumps[final_state.position]) assert(final_state.position + distance > goal);

    //  hashが位置なので、各層に残るのはスコア最良の1状態だけ。幅1でもDPと同じになる。
    //  同じ位置へ別々の層から候補が届くので、層をまたいで届いた候補の重複排除も通る
    frog::Summary summary = frog::summarize();
    assert(std::abs(with_undo.score - summary.optimum) < 1e-6);

    coverage.empty_layer += summary.has_empty_layer;
    coverage.dropped_transition += summary.drops_transition;
    coverage.answer_before_goal += with_undo.layer < goal;
    coverage.answer_at_goal += with_undo.layer == goal;
}

void solve() {
    //  全stepが1の場合。幅を絞っても、max_turnを経路より長くしても一致し続ける
    for (int width: {1, 2, 7, 100}) {
        test_same_as_beam_search(1, width, grid::H + grid::W - 2);
        test_same_as_beam_search(2, width, grid::H + grid::W - 1);
        test_same_as_beam_search(3, width, 10);
    }

    //  幅を十分大きくすればDP厳密解に一致する(1層あたりの状態数は高々H)
    test_jump_grid(1, 100, grid::H + grid::W - 2, true);
    test_jump_grid(2, 100, grid::H + grid::W - 2, true);
    //  max_layerを経路より長くして、終端検出の経路も通す
    test_jump_grid(3, 100, grid::H + grid::W + 3, true);
    //  max_layerを経路より短くする。最後の層を飛び越す遷移は捨てられる
    test_jump_grid(4, 100, 10, true);
    test_jump_grid(5, 100, 1, true);
    //  幅を絞ると最適とは限らないが、2つの版は一致し続ける
    test_jump_grid(6, 1, grid::H + grid::W - 2, false);
    test_jump_grid(7, 2, grid::H + grid::W - 2, false);
    test_jump_grid(8, 7, grid::H + grid::W + 3, false);
    test_jump_grid(9, 3, 9, false);

    test_root_only();

    FrogCoverage coverage;
    for (unsigned seed = 1; seed <= 60; seed++) {
        test_frog(seed, 30, 1, coverage);
        test_frog(seed, 1 + seed % 7, 3, coverage);
    }
    assert(coverage.empty_layer > 0);
    assert(coverage.dropped_transition > 0);
    assert(coverage.answer_before_goal > 0);
    assert(coverage.answer_at_goal > 0);
}

}  //  namespace nono

int main() {
    nono::solve();
}
