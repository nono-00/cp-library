// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <vector>

#include "nono/heuristic/beam-search.hpp"
#include "nono/heuristic/chokudai-search.hpp"

namespace nono {

using Hash = unsigned long long;

//  ---------------- グリッド経路最大化 ----------------
//  DPで厳密解が求まる。旧実装はこの問題で最適解に到達できなかった

namespace grid {

constexpr int H = 14, W = 14;
double value[H][W];

struct Action {
    int direction;  //  0:右 1:下
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
    state.row += (action.direction == 1);
    state.column += (action.direction == 0);
    state.score += value[state.row][state.column];
}

void undo(State& state, const Action& action) {
    state.score -= value[state.row][state.column];
    state.row -= (action.direction == 1);
    state.column -= (action.direction == 0);
}

template <class F>
void expand(const State& state, F f) {
    if (state.column + 1 < W) {
        f(Action{0}, state.score + value[state.row][state.column + 1], hash_of(state.row, state.column + 1));
    }
    if (state.row + 1 < H) {
        f(Action{1}, state.score + value[state.row + 1][state.column], hash_of(state.row + 1, state.column));
    }
}

double optimum() {
    static double dp[H][W];
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            double best = -1e18;
            if (i == 0 && j == 0) best = 0;
            if (i > 0) best = std::max(best, dp[i - 1][j]);
            if (j > 0) best = std::max(best, dp[i][j - 1]);
            dp[i][j] = value[i][j] + best;
        }
    }
    return dp[H - 1][W - 1];
}

double replay(const std::vector<Action>& actions) {
    State state{0, 0, value[0][0]};
    for (const Action& action: actions) apply(state, action);
    assert(state.row == H - 1 && state.column == W - 1);
    return state.score;
}

}  //  namespace grid

void test_grid(unsigned seed, int width, double time_limit, bool expect_optimal) {
    grid::initialize(seed);
    double optimum = grid::optimum();
    grid::State initial{0, 0, grid::value[0][0]};
    ChokudaiSearchConfig config{grid::H + grid::W - 1, width, time_limit};

    auto result = chokudai_search<grid::Action, Hash>(initial, initial.score, config, [](const grid::State& s, auto f) {
        grid::expand(s, f);
    }, grid::apply, grid::undo);

    //  終端(ゴール)に到達していること
    assert((int)result.actions.size() == grid::H + grid::W - 2);
    //  報告スコアが、返ったAction列を辿り直した値と一致すること
    assert(std::abs(grid::replay(result.actions) - result.score) < 1e-6);
    //  最適値を超えることはない
    assert(result.score <= optimum + 1e-6);
    assert(result.passes >= 1);
    if (expect_optimal) assert(result.score > optimum - 1e-6);
}

//  十分な時間と幅があれば、ビームサーチと同じ最適解に到達すること
void test_matches_beam(unsigned seed) {
    grid::initialize(seed);
    grid::State initial{0, 0, grid::value[0][0]};

    auto beam = beam_search<grid::Action, Hash>(initial, initial.score, BeamSearchConfig{grid::H + grid::W - 1, 100},
                                                [](const grid::State& s, auto f) { grid::expand(s, f); }, grid::apply,
                                                grid::undo);
    auto chokudai = chokudai_search<grid::Action, Hash>(
        initial, initial.score, ChokudaiSearchConfig{grid::H + grid::W - 1, 5, 100},
        [](const grid::State& s, auto f) { grid::expand(s, f); }, grid::apply, grid::undo);

    assert(std::abs(beam.score - chokudai.score) < 1e-6);
}

//  ---------------- 可変長の木 ----------------
//  葉の深さがばらばらなので、終端検出が効く

namespace tree_problem {

struct Action {
    int child;
};

struct State {
    int node;
    double score;
};

std::vector<std::vector<std::pair<int, double>>> children;
std::vector<int> parent_of;
std::vector<double> weight_from_parent;

void initialize(unsigned seed, int node_count) {
    std::mt19937 mt(seed);
    children.assign(node_count, {});
    parent_of.assign(node_count, -1);
    weight_from_parent.assign(node_count, 0);
    for (int i = 1; i < node_count; i++) {
        int parent = mt() % i;
        while (children[parent].size() >= 3 && parent_of[parent] != -1) parent = parent_of[parent];
        if (children[parent].size() >= 3) continue;
        double weight = (mt() % 100) + 0.5;
        children[parent].push_back({i, weight});
        parent_of[i] = parent;
        weight_from_parent[i] = weight;
    }
}

void apply(State& state, const Action& action) {
    auto [child, weight] = children[state.node][action.child];
    state.node = child;
    state.score += weight;
}

void undo(State& state, const Action&) {
    state.score -= weight_from_parent[state.node];
    state.node = parent_of[state.node];
}

template <class F>
void expand(const State& state, F f) {
    for (int i = 0; i < (int)children[state.node].size(); i++) {
        auto [child, weight] = children[state.node][i];
        f(Action{i}, state.score + weight, static_cast<Hash>(child));
    }
}

double optimum(int node) {
    if (children[node].empty()) return 0;
    double best = -1e18;
    for (auto [child, weight]: children[node]) best = std::max(best, weight + optimum(child));
    return best;
}

double replay(const std::vector<Action>& actions) {
    State state{0, 0};
    for (const Action& action: actions) apply(state, action);
    assert(children[state.node].empty());
    return state.score;
}

}  //  namespace tree_problem

void test_tree(unsigned seed, int node_count, int width) {
    tree_problem::initialize(seed, node_count);
    double optimum = tree_problem::optimum(0);
    tree_problem::State initial{0, 0};
    ChokudaiSearchConfig config{node_count + 1, width, 100};

    auto result = chokudai_search<tree_problem::Action, Hash>(
        initial, 0.0, config, [](const tree_problem::State& s, auto f) { tree_problem::expand(s, f); },
        tree_problem::apply, tree_problem::undo);

    assert(std::abs(tree_problem::replay(result.actions) - result.score) < 1e-6);
    assert(result.score > optimum - 1e-6);
    assert(result.score < optimum + 1e-6);
}

//  max_turnが足りず終端に到達できない場合でも、到達した中から答えを返すこと
void test_truncated() {
    grid::initialize(1);
    grid::State initial{0, 0, grid::value[0][0]};
    ChokudaiSearchConfig config{5, 3, 50};
    auto result = chokudai_search<grid::Action, Hash>(initial, initial.score, config, [](const grid::State& s, auto f) {
        grid::expand(s, f);
    }, grid::apply, grid::undo);
    assert((int)result.actions.size() == 5);
    grid::State state{0, 0, grid::value[0][0]};
    for (const auto& action: result.actions) grid::apply(state, action);
    assert(std::abs(state.score - result.score) < 1e-6);
}

void solve() {
    //  幅と時間が十分あればDP厳密解に到達する。
    //  旧実装はmax_turn = turnのせいでゴール階層が2パス目以降から外れ、
    //  最初に見つけたゴールで答えが凍結していた
    test_grid(1, 5, 100, true);
    test_grid(2, 5, 100, true);
    test_grid(3, 20, 100, true);
    //  幅1でも報告スコアと経路の整合は保たれる
    test_grid(4, 1, 50, false);

    test_matches_beam(5);
    test_tree(1, 200, 5);
    test_tree(2, 400, 3);
    test_truncated();
}

}  //  namespace nono

int main() {
    nono::solve();
}
