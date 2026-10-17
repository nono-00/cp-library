// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <limits>
#include <map>
#include <random>
#include <tuple>
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

//  ---------------- 素朴な実装との比較 ----------------
//  ランダムな有向グラフを辺に沿って進む。辺ごとに重みと進む層数(1〜3)が決まっている。
//  hashは今いる頂点なので、同じ層の同じ頂点に着いた経路は1本にまとめられる。
//  出次数0の頂点や、max_layerを超える辺しかない頂点が終端になる。
//  重みは整数で、和を整数のまま持つ(undoで誤差が溜まらない)。値域が広いので同点はまず起きず、
//  素朴な実装と答えが一意に一致する

namespace graph {

struct Edge {
    int to;
    long long weight;
    int step;
};

struct Action {
    int from;
    int edge;
    bool operator==(const Action&) const = default;
};

struct State {
    int node;
    long long score;
    int layer;
};

std::vector<std::vector<Edge>> edges;
//  展開した順の(層, 頂点, スコア)。展開の順序も素朴な実装と比べる
std::vector<std::tuple<int, int, long long>> trace;

void initialize(unsigned seed, int node_count) {
    std::mt19937 mt(seed);
    edges.assign(node_count, {});
    for (int i = 0; i < node_count; i++) {
        //  根以外の出次数0は1/5。根で終わると何も確かめられないので、根は4本にする
        int degree = i == 0 ? 4 : mt() % 5;
        for (int k = 0; k < degree; k++) {
            int to = mt() % node_count;
            long long weight = static_cast<long long>(mt() % 2000001) - 1000000;
            int step = 1 + mt() % 3;
            edges[i].push_back({to, weight, step});
        }
    }
}

void apply(State& state, const Action& action) {
    const Edge& edge = edges[action.from][action.edge];
    state.node = edge.to;
    state.score += edge.weight;
    state.layer += edge.step;
}

void undo(State& state, const Action& action) {
    const Edge& edge = edges[action.from][action.edge];
    state.node = action.from;
    state.score -= edge.weight;
    state.layer -= edge.step;
}

template <class F>
void expand(const State& state, F f) {
    trace.push_back({state.layer, state.node, state.score});
    for (int e = 0; e < static_cast<int>(edges[state.node].size()); e++) {
        const Edge& edge = edges[state.node][e];
        f(Action{state.node, e}, static_cast<double>(state.score + edge.weight), static_cast<Hash>(edge.to), edge.step);
    }
}

State replay(const std::vector<Action>& actions) {
    State state{0, 0, 0};
    for (const Action& action: actions) {
        assert(action.from == state.node);
        apply(state, action);
    }
    return state;
}

//  層ごとに、全候補のStateを作り、hashごとにスコア最良(同点なら先に来た方)を残し、
//  スコア上位width件を候補の生成順に並べて展開する
LayeredBeamSearchResult<Action> naive_layered_beam_search(int max_layer, int width, double goal_score) {
    struct Node {
        std::vector<Action> path;
        State state;
        double score;
        Hash hash;
    };
    std::vector<std::vector<Node>> pools(max_layer + 1);
    std::vector<Node> frontier{{{}, State{0, 0, 0}, 0.0, 0}};
    LayeredBeamSearchResult<Action> best{{}, std::numeric_limits<double>::lowest(), 0};
    auto update = [&](const Node& node, int layer) {
        if (node.score > best.score) best = {node.path, node.score, layer};
    };
    bool goal = 0.0 >= goal_score;
    for (int layer = 0; layer <= max_layer; layer++) {
        if (layer > 0) {
            const auto& pool = pools[layer];
            std::map<Hash, int> position;
            std::vector<int> kept;
            for (int i = 0; i < static_cast<int>(pool.size()); i++) {
                auto it = position.find(pool[i].hash);
                if (it == position.end()) {
                    position[pool[i].hash] = static_cast<int>(kept.size());
                    kept.push_back(i);
                } else if (pool[kept[it->second]].score < pool[i].score) {
                    kept[it->second] = i;
                }
            }
            std::sort(kept.begin(), kept.end(), [&](int l, int r) { return pool[l].score > pool[r].score; });
            if (static_cast<int>(kept.size()) > width) kept.resize(width);
            std::sort(kept.begin(), kept.end());
            frontier.clear();
            for (int i: kept) {
                frontier.push_back(pool[i]);
                if (pool[i].score >= goal_score) goal = true;
            }
        }
        if (layer == max_layer || goal) {
            for (const Node& node: frontier) update(node, layer);
            break;
        }
        for (const Node& node: frontier) {
            int produced = 0;
            expand(node.state, [&](const Action& action, double score, Hash hash, int step) {
                if (step > max_layer - layer) return;
                produced++;
                Node child{node.path, node.state, score, hash};
                child.path.push_back(action);
                apply(child.state, action);
                pools[layer + step].push_back(child);
            });
            if (produced == 0) update(node, layer);
        }
    }
    return best;
}

}  //  namespace graph

//  test_same_as_naiveが通した経路の数。狙った経路を1回も通らないままテストが通ることを防ぐ
struct NaiveCoverage {
    int stopped_by_goal = 0;
    int answer_at_terminal = 0;
    int answer_at_max_layer = 0;
};

void test_same_as_naive(unsigned seed, int node_count, int width, int max_layer, bool with_goal,
                        NaiveCoverage& coverage) {
    graph::initialize(seed, node_count);
    double goal_score = std::numeric_limits<double>::infinity();
    if (with_goal) {
        //  打ち切りなしの答えの半分をゴールにすると、途中の層で打ち切られる。
        //  答えが正でなければ、初期状態(スコア0)がゴールになってしまうので打ち切らない
        double unlimited = graph::naive_layered_beam_search(max_layer, width, goal_score).score;
        if (unlimited > 0) goal_score = unlimited / 2;
    }
    graph::trace.clear();
    auto expected = graph::naive_layered_beam_search(max_layer, width, goal_score);
    auto expected_trace = graph::trace;

    LayeredBeamSearchConfig config{max_layer, width};
    config.goal_score = goal_score;
    auto expand = [](const graph::State& s, auto f) {
        graph::expand(s, f);
    };
    for (bool use_undo: {true, false}) {
        graph::trace.clear();
        auto result = use_undo ? layered_beam_search<graph::Action, Hash>(graph::State{0, 0, 0}, 0.0, config, expand,
                                                                          graph::apply, graph::undo)
                               : layered_beam_search<graph::Action, Hash>(graph::State{0, 0, 0}, 0.0, config, expand,
                                                                          graph::apply);
        assert(result.actions == expected.actions);
        assert(result.score == expected.score);
        assert(result.layer == expected.layer);
        //  各層のノードを、素朴な実装と同じ順に展開する
        assert(graph::trace == expected_trace);
        graph::State final_state = graph::replay(result.actions);
        assert(static_cast<double>(final_state.score) == result.score && final_state.layer == result.layer);
    }
    if (expected.score >= goal_score) {
        assert(expected.layer < max_layer);
        coverage.stopped_by_goal++;
    } else if (expected.layer < max_layer) {
        coverage.answer_at_terminal++;
    } else {
        coverage.answer_at_max_layer++;
    }
}

//  ---------------- 同点のときの答え ----------------
//  子を明示した小さな木。各ノードは絶対スコアと、親から何層進むかを持つ

namespace small_tree {

struct Node {
    double score;
    int step;
    std::vector<int> children;
};

std::vector<Node> nodes;

struct Action {
    int to;
};

struct State {
    int node;
    std::vector<int> history;  //  undo用に通ったノードを積む
};

void apply(State& state, const Action& action) {
    state.history.push_back(state.node);
    state.node = action.to;
}

void undo(State& state, const Action&) {
    state.node = state.history.back();
    state.history.pop_back();
}

template <class F>
void expand(const State& state, F f) {
    for (int child: nodes[state.node].children) {
        f(Action{child}, nodes[child].score, static_cast<Hash>(child), nodes[child].step);
    }
}

std::vector<int> run(int max_layer, bool use_undo) {
    LayeredBeamSearchConfig config{max_layer, 10};
    auto e = [](const State& s, auto f) {
        expand(s, f);
    };
    auto result = use_undo ? layered_beam_search<Action, Hash>(State{0, {}}, nodes[0].score, config, e, apply, undo)
                           : layered_beam_search<Action, Hash>(State{0, {}}, nodes[0].score, config, e, apply);
    std::vector<int> path;
    for (const Action& action: result.actions) path.push_back(action.to);
    return path;
}

}  //  namespace small_tree

void test_ties() {
    using small_tree::nodes;
    for (bool use_undo: {true, false}) {
        //  0 -> 1(5, 層1, 終端), 0 -> 2(1, 層1) -> 3(5, 層3, 終端)。同点の終端なら前の層
        nodes = {{0, 0, {1, 2}}, {5, 1, {}}, {1, 1, {3}}, {5, 2, {}}};
        assert(small_tree::run(10, use_undo) == std::vector<int>({1}));
        //  0 -> 1(1, 層1) -> {2(7, 層2), 3(7, 層2)}。最後の層の候補同士では先に作られた方
        nodes = {{0, 0, {1}}, {1, 1, {2, 3}}, {7, 1, {}}, {7, 1, {}}};
        assert(small_tree::run(2, use_undo) == std::vector<int>({1, 2}));
        nodes = {{0, 0, {1}}, {1, 1, {3, 2}}, {7, 1, {}}, {7, 1, {}}};
        assert(small_tree::run(2, use_undo) == std::vector<int>({1, 3}));
        //  0 -> 1(1, 層1) -> 2(7, 層2), 0 -> 3(7, 層2)。層0から配った3の方が先に作られる
        nodes = {{0, 0, {1, 3}}, {1, 1, {2}}, {7, 1, {}}, {7, 2, {}}};
        assert(small_tree::run(2, use_undo) == std::vector<int>({3}));
    }
}

//  ---------------- 打ち切り ----------------
//  各ノードから1層先と2層先へ進める1本道。スコアは層。
//  層5に着いた候補が選ばれた時点で止まり、層6へ配られた候補は見ないこと

namespace chain {

struct Action {
    int step;
};

struct State {
    int layer;
};

int max_expanded_layer;

void apply(State& state, const Action& action) {
    state.layer += action.step;
}

void undo(State& state, const Action& action) {
    state.layer -= action.step;
}

template <class F>
void expand(const State& state, F f) {
    max_expanded_layer = std::max(max_expanded_layer, state.layer);
    for (int step = 1; step <= 2; step++) {
        f(Action{step}, state.layer + step, static_cast<Hash>(state.layer + step), step);
    }
}

}  //  namespace chain

void test_goal_score() {
    auto expand = [](const chain::State& s, auto f) {
        chain::expand(s, f);
    };
    for (bool use_undo: {true, false}) {
        auto run = [&](const LayeredBeamSearchConfig& config, double initial_score) {
            chain::max_expanded_layer = -1;
            return use_undo ? layered_beam_search<chain::Action, Hash>(chain::State{0}, initial_score, config, expand,
                                                                       chain::apply, chain::undo)
                            : layered_beam_search<chain::Action, Hash>(chain::State{0}, initial_score, config, expand,
                                                                       chain::apply);
        };
        LayeredBeamSearchConfig config{100, 10};
        config.goal_score = 5;
        auto result = run(config, 0);
        assert(result.layer == 5 && result.score == 5);
        //  層5のノードは展開しない
        assert(chain::max_expanded_layer == 4);

        //  初期状態がゴールなら何もしない
        result = run(config, 5);
        assert(result.actions.empty() && result.score == 5 && result.layer == 0);
        assert(chain::max_expanded_layer == -1);

        //  指定しなければmax_layerまで進む
        result = run(LayeredBeamSearchConfig{100, 10}, 0);
        assert(result.layer == 100);
    }
}

//  ---------------- 時間で幅を決める ----------------
//  4分岐の木。子番号が偶数なら1層、奇数なら2層進む。hashは候補ごとに違うので重複排除は起きない。
//  1ノードの展開に一定時間かかるようにして、残す候補数が時間で決まることを確かめる

namespace timed {

struct Action {
    int child;
};

struct State {
    int layer;
    unsigned long long key;
};

constexpr double NODE_MICROSECONDS = 2;
//  goal_layerに着いたらスコアを大きくする。0ならゴールなし
int goal_layer;
std::vector<int> expanded_per_layer;

int step_of(int child) {
    return 1 + child % 2;
}

void apply(State& state, const Action& action) {
    state.layer += step_of(action.child);
    state.key = state.key * 4 + action.child;
}

void undo(State& state, const Action& action) {
    state.layer -= step_of(action.child);
    state.key /= 4;
}

unsigned long long mix(unsigned long long x) {
    x ^= x >> 31;
    x *= 0x7fb5d329728ea185ull;
    x ^= x >> 27;
    return x;
}

template <class F>
void expand(const State& state, F f) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count()
           < NODE_MICROSECONDS) {}
    if (static_cast<int>(expanded_per_layer.size()) <= state.layer) expanded_per_layer.resize(state.layer + 1);
    expanded_per_layer[state.layer]++;
    for (int child = 0; child < 4; child++) {
        unsigned long long key = state.key * 4 + child;
        double score = static_cast<double>(mix(key) % 1000);
        if (goal_layer > 0 && state.layer + step_of(child) == goal_layer) score += 1e6;
        f(Action{child}, score, static_cast<Hash>(mix(key ^ 0x9e3779b97f4a7c15ull)), step_of(child));
    }
}

struct Run {
    LayeredBeamSearchResult<Action> result;
    double elapsed;
};

Run run(const LayeredBeamSearchConfig& config, bool use_undo) {
    expanded_per_layer.clear();
    auto e = [](const State& s, auto f) {
        expand(s, f);
    };
    auto start = std::chrono::steady_clock::now();
    auto result = use_undo ? layered_beam_search<Action, Hash>(State{0, 1}, 0, config, e, apply, undo)
                           : layered_beam_search<Action, Hash>(State{0, 1}, 0, config, e, apply);
    return {result, std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count()};
}

}  //  namespace timed

void test_time_limit() {
    timed::goal_layer = 0;
    for (bool use_undo: {true, false}) {
        //  制限時間をほぼ使い切り、それを大きく超えない
        LayeredBeamSearchConfig config{100, 20000, 100};
        auto run = timed::run(config, use_undo);
        assert(run.result.layer == 100);
        assert(50 < run.elapsed && run.elapsed < 150);
        //  1層あたり1ミリ秒、1ノード2マイクロ秒なので、幅は数百になる
        int middle = timed::expanded_per_layer[50];
        assert(50 < middle && middle < 5000);

        //  beam_widthを超えない
        config.beam_width = 30;
        run = timed::run(config, use_undo);
        for (int count: timed::expanded_per_layer) assert(count <= 30);
        assert(run.elapsed < 100);

        //  時間を使い切った後は1つずつ残して最後まで進む
        config = LayeredBeamSearchConfig{2000, 20000, 1};
        run = timed::run(config, use_undo);
        assert(run.result.layer == 2000);
        assert(run.elapsed < 100);
    }
}

void test_expected_layer() {
    timed::goal_layer = 50;
    for (bool use_undo: {true, false}) {
        LayeredBeamSearchConfig config{500, 20000, 100};
        config.goal_score = 1e6;
        //  max_layerまで続くとみなすと、層50で打ち切った時点で時間の1割程度しか使っていない
        auto run = timed::run(config, use_undo);
        assert(run.result.layer == 50);
        assert(run.elapsed < 50);
        //  層50までと見込めば、時間の大半を使う
        config.expected_layer = 50;
        run = timed::run(config, use_undo);
        assert(run.result.layer == 50);
        assert(50 < run.elapsed && run.elapsed < 150);
    }
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

    //  素朴な実装と答え・展開の順序が一致する(層を飛び越す遷移・重複排除・終端・打ち切りを含む)
    NaiveCoverage naive_coverage;
    for (unsigned seed = 1; seed <= 6; seed++) {
        for (int width: {1, 3, 20, 200}) {
            test_same_as_naive(seed, 300, width, 30, false, naive_coverage);
            test_same_as_naive(seed, 300, width, 30, true, naive_coverage);
        }
    }
    assert(naive_coverage.stopped_by_goal > 0);
    assert(naive_coverage.answer_at_terminal > 0);
    assert(naive_coverage.answer_at_max_layer > 0);
    test_ties();
    test_goal_score();
    test_time_limit();
    test_expected_layer();
}

}  //  namespace nono

int main() {
    nono::solve();
}
