// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <vector>

#include "nono/heuristic/beam-search.hpp"

namespace nono {

using Hash = unsigned long long;

//  ---------------- グリッド経路最大化 ----------------
//  (0,0)から(H-1,W-1)へ右/下に進み、通ったマスの値の総和を最大化する。
//  DPで厳密解が求まるので、ビーム幅を十分大きくすれば一致するはず

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

///  次の状態を作らずにscoreとhashを渡す
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

///  返ってきたActionの列を実際に辿り直したスコア
double replay(const std::vector<Action>& actions) {
    State state{0, 0, value[0][0]};
    for (const Action& action: actions) apply(state, action);
    assert(state.row == H - 1 && state.column == W - 1);
    return state.score;
}

}  //  namespace grid

void test_grid(unsigned seed, int width, int max_turn, bool expect_optimal) {
    grid::initialize(seed);
    double optimum = grid::optimum();
    grid::State initial{0, 0, grid::value[0][0]};
    BeamSearchConfig config{max_turn, width};

    auto with_undo = beam_search<grid::Action, Hash>(initial, initial.score, config, [](const grid::State& s, auto f) {
        grid::expand(s, f);
    }, grid::apply, grid::undo);
    auto without_undo = beam_search<grid::Action, Hash>(
        initial, initial.score, config, [](const grid::State& s, auto f) { grid::expand(s, f); }, grid::apply);

    //  報告されたスコアが、実際に辿り直したスコアと一致すること
    assert(std::abs(grid::replay(with_undo.actions) - with_undo.score) < 1e-6);
    assert(std::abs(grid::replay(without_undo.actions) - without_undo.score) < 1e-6);
    //  手数は必ずH+W-2
    assert((int)with_undo.actions.size() == grid::H + grid::W - 2);
    assert((int)without_undo.actions.size() == grid::H + grid::W - 2);

    //  2つのポリシーは完全に同じ答えを返すこと
    assert(with_undo.score == without_undo.score);
    assert(with_undo.actions.size() == without_undo.actions.size());
    for (std::size_t i = 0; i < with_undo.actions.size(); i++) {
        assert(with_undo.actions[i].direction == without_undo.actions[i].direction);
    }

    assert(with_undo.score <= optimum + 1e-6);
    if (expect_optimal) assert(with_undo.score > optimum - 1e-6);
}

//  ---------------- 可変長の木 ----------------
//  子が0〜3個のランダムな木。根から葉への経路の重み和を最大化する。
//  葉に到達すると展開先がなくなるので、終端検出を通る

namespace tree_problem {

struct Action {
    int child;
};

struct State {
    int node;
    double score;
};

std::vector<std::vector<std::pair<int, double>>> children;  //  node -> (子node, 辺の重み)
std::vector<int> parent_of;
std::vector<double> weight_from_parent;

void initialize(unsigned seed, int node_count) {
    std::mt19937 mt(seed);
    children.assign(node_count, {});
    parent_of.assign(node_count, -1);
    weight_from_parent.assign(node_count, 0);
    for (int i = 1; i < node_count; i++) {
        int parent = mt() % i;
        if (children[parent].size() >= 3) {
            //  子が3個を超えないように、空きのある祖先へ付け替える
            while (children[parent].size() >= 3 && parent_of[parent] != -1) parent = parent_of[parent];
            if (children[parent].size() >= 3) continue;
        }
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

void undo(State& state, const Action& action) {
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

///  根から葉への最大重み和
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
    //  木の深さより十分大きくしておく
    BeamSearchConfig config{node_count + 1, width};

    auto with_undo = beam_search<tree_problem::Action, Hash>(
        initial, 0.0, config, [](const tree_problem::State& s, auto f) { tree_problem::expand(s, f); },
        tree_problem::apply, tree_problem::undo);
    auto without_undo = beam_search<tree_problem::Action, Hash>(
        initial, 0.0, config, [](const tree_problem::State& s, auto f) { tree_problem::expand(s, f); },
        tree_problem::apply);

    assert(std::abs(tree_problem::replay(with_undo.actions) - with_undo.score) < 1e-6);
    assert(with_undo.score == without_undo.score);
    //  幅が十分あれば全探索と同じになる
    assert(with_undo.score > optimum - 1e-6);
    assert(with_undo.score < optimum + 1e-6);
}

void solve() {
    //  幅を十分大きくすればDP厳密解に一致する(1手あたりの状態数は高々H+W)
    test_grid(1, 100, grid::H + grid::W - 2, true);
    test_grid(2, 100, grid::H + grid::W - 2, true);
    //  max_turnを1手多くして、終端検出の経路も通す
    test_grid(3, 100, grid::H + grid::W - 1, true);
    //  幅を絞ると最適とは限らないが、2つのポリシーは一致し続ける
    test_grid(4, 1, grid::H + grid::W - 2, false);
    test_grid(5, 2, grid::H + grid::W - 2, false);
    test_grid(6, 7, grid::H + grid::W - 1, false);

    //  可変長の木。葉の深さがばらばらなので終端検出が効く
    test_tree(1, 200, 1000);
    test_tree(2, 500, 1000);
    test_tree(3, 60, 1000);
}

}  //  namespace nono

int main() {
    nono::solve();
}
