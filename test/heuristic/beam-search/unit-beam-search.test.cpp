// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <limits>
#include <map>
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

//  次の状態を作らずにscoreとhashを渡す
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

//  返ってきたActionの列を実際に辿り直したスコア
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

//  根から葉への最大重み和
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

//  ---------------- 素朴な実装との比較 ----------------
//  ランダムな有向グラフを辺に沿って進み、辺の重みの和を最大化する。
//  hashは今いる頂点なので、同じ手で同じ頂点に着いた経路は1本にまとめられる。
//  出次数0の頂点では展開先がなくなり、終端になる。
//  重みは整数で、和を整数のまま持つ(undoで誤差が溜まらない)。値域が広いので同点はまず起きず、
//  素朴な実装と答えが一意に一致する

namespace graph {

struct Action {
    int from;
    int edge;
};

struct State {
    int node;
    long long score;
};

std::vector<std::vector<std::pair<int, long long>>> edges;  //  node -> (行き先, 重み)

void initialize(unsigned seed, int node_count) {
    std::mt19937 mt(seed);
    edges.assign(node_count, {});
    for (int i = 0; i < node_count; i++) {
        //  根以外の出次数0は1/5。終端が適度に出る。根で終わると何も確かめられないので、根は4本にする
        int degree = i == 0 ? 4 : mt() % 5;
        for (int k = 0; k < degree; k++) {
            edges[i].push_back({static_cast<int>(mt() % node_count), static_cast<long long>(mt() % 2000001) - 1000000});
        }
    }
}

void apply(State& state, const Action& action) {
    auto [to, weight] = edges[action.from][action.edge];
    state.node = to;
    state.score += weight;
}

void undo(State& state, const Action& action) {
    state.score -= edges[action.from][action.edge].second;
    state.node = action.from;
}

template <class F>
void expand(const State& state, F f) {
    for (int e = 0; e < static_cast<int>(edges[state.node].size()); e++) {
        auto [to, weight] = edges[state.node][e];
        f(Action{state.node, e}, static_cast<double>(state.score + weight), static_cast<Hash>(to));
    }
}

double replay(const std::vector<Action>& actions) {
    State state{0, 0};
    for (const Action& action: actions) {
        assert(action.from == state.node);
        apply(state, action);
    }
    return static_cast<double>(state.score);
}

//  1手ごとに、全候補のStateを作り、hashごとにスコア最良(同点なら先に来た方)を残し、
//  スコア上位width件を候補の生成順に並べて次の手に進む
BeamSearchResult<Action> naive_beam_search(int max_turn, int width, double goal_score) {
    struct Node {
        std::vector<Action> path;
        State state;
        double score;
    };
    std::vector<Node> frontier{{{}, State{0, 0}, 0.0}};
    Node best{{}, State{0, 0}, std::numeric_limits<double>::lowest()};
    bool goal = 0.0 >= goal_score;
    for (int turn = 0; turn < max_turn && !goal; turn++) {
        std::vector<Node> candidates;
        std::vector<Hash> hashes;
        for (const Node& node: frontier) {
            int produced = 0;
            expand(node.state, [&](const Action& action, double score, Hash hash) {
                produced++;
                Node child = node;
                child.path.push_back(action);
                apply(child.state, action);
                child.score = score;
                candidates.push_back(child);
                hashes.push_back(hash);
            });
            if (produced == 0 && node.score > best.score) best = node;
        }
        std::map<Hash, int> position;
        std::vector<int> kept;
        for (int i = 0; i < static_cast<int>(candidates.size()); i++) {
            auto it = position.find(hashes[i]);
            if (it == position.end()) {
                position[hashes[i]] = static_cast<int>(kept.size());
                kept.push_back(i);
            } else if (candidates[kept[it->second]].score < candidates[i].score) {
                kept[it->second] = i;
            }
        }
        if (kept.empty()) break;
        std::sort(kept.begin(), kept.end(), [&](int l, int r) { return candidates[l].score > candidates[r].score; });
        if (static_cast<int>(kept.size()) > width) kept.resize(width);
        std::sort(kept.begin(), kept.end());
        frontier.clear();
        for (int i: kept) {
            frontier.push_back(candidates[i]);
            if (candidates[i].score >= goal_score) goal = true;
        }
    }
    for (const Node& node: frontier) {
        if (node.score > best.score) best = node;
    }
    return BeamSearchResult<Action>{best.path, best.score};
}

}  //  namespace graph

//  test_same_as_naiveが通した経路の数。狙った経路を1回も通らないままテストが通ることを防ぐ
struct NaiveCoverage {
    int stopped_by_goal = 0;
    int answer_at_terminal = 0;
    int answer_at_max_turn = 0;
};

void test_same_as_naive(unsigned seed, int node_count, int width, int max_turn, bool with_goal,
                        NaiveCoverage& coverage) {
    graph::initialize(seed, node_count);
    double goal_score = std::numeric_limits<double>::infinity();
    if (with_goal) {
        //  打ち切りなしの答えの半分をゴールにすると、途中の手で打ち切られる。
        //  答えが正でなければ、初期状態(スコア0)がゴールになってしまうので打ち切らない
        double unlimited = graph::naive_beam_search(max_turn, width, goal_score).score;
        if (unlimited > 0) goal_score = unlimited / 2;
    }
    auto expected = graph::naive_beam_search(max_turn, width, goal_score);
    BeamSearchConfig config{max_turn, width};
    config.goal_score = goal_score;
    auto expand = [](const graph::State& s, auto f) {
        graph::expand(s, f);
    };
    auto with_undo =
        beam_search<graph::Action, Hash>(graph::State{0, 0}, 0.0, config, expand, graph::apply, graph::undo);
    auto without_undo = beam_search<graph::Action, Hash>(graph::State{0, 0}, 0.0, config, expand, graph::apply);

    for (const auto& result: {with_undo, without_undo}) {
        assert(result.score == expected.score);
        assert(result.actions.size() == expected.actions.size());
        for (std::size_t i = 0; i < result.actions.size(); i++) {
            assert(result.actions[i].from == expected.actions[i].from);
            assert(result.actions[i].edge == expected.actions[i].edge);
        }
        assert(graph::replay(result.actions) == result.score);
    }
    if (expected.score >= goal_score) {
        //  打ち切ったなら、答えの手数は打ち切らない場合より短い
        assert(static_cast<int>(expected.actions.size()) < max_turn);
        coverage.stopped_by_goal++;
    } else if (static_cast<int>(expected.actions.size()) < max_turn) {
        coverage.answer_at_terminal++;
    } else {
        coverage.answer_at_max_turn++;
    }
}

//  ---------------- 同点のときの答え ----------------
//  子を明示した小さな木。scoreは各ノードの絶対スコア

namespace small_tree {

struct Node {
    double score;
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
    for (int child: nodes[state.node].children) f(Action{child}, nodes[child].score, static_cast<Hash>(child));
}

std::vector<int> run(int max_turn, int width, bool use_undo) {
    BeamSearchConfig config{max_turn, width};
    auto e = [](const State& s, auto f) {
        expand(s, f);
    };
    auto result = use_undo ? beam_search<Action, Hash>(State{0, {}}, nodes[0].score, config, e, apply, undo)
                           : beam_search<Action, Hash>(State{0, {}}, nodes[0].score, config, e, apply);
    std::vector<int> path;
    for (const Action& action: result.actions) path.push_back(action.to);
    return path;
}

}  //  namespace small_tree

void test_ties() {
    using small_tree::nodes;
    for (bool use_undo: {true, false}) {
        //  0 -> 1(5, 終端), 0 -> 2(1) -> 3(5, 終端)。同点の終端なら浅い方
        nodes = {{0, {1, 2}}, {5, {}}, {1, {3}}, {5, {}}};
        assert(small_tree::run(10, 10, use_undo) == std::vector<int>({1}));
        //  0 -> 1(5, 終端), 0 -> 2(1) -> 3(5)。最後に残った候補より終端が先
        nodes = {{0, {1, 2}}, {5, {}}, {1, {3}}, {5, {4}}, {0, {}}};
        assert(small_tree::run(2, 10, use_undo) == std::vector<int>({1}));
        //  0 -> 1(1) -> {2(7), 3(7)}。最後に残った候補同士ではDFS順で先の方
        nodes = {{0, {1}}, {1, {2, 3}}, {7, {}}, {7, {}}};
        assert(small_tree::run(2, 10, use_undo) == std::vector<int>({1, 2}));
        nodes = {{0, {1}}, {1, {3, 2}}, {7, {}}, {7, {}}};
        assert(small_tree::run(2, 10, use_undo) == std::vector<int>({1, 3}));
    }
}

//  ---------------- 打ち切り ----------------
//  1本道で、スコアは深さ。depth >= goal_score になった手で止まること

namespace chain {

struct Action {};

struct State {
    int depth;
};

int max_expanded_depth;

void apply(State& state, const Action&) {
    state.depth++;
}

void undo(State& state, const Action&) {
    state.depth--;
}

template <class F>
void expand(const State& state, F f) {
    max_expanded_depth = std::max(max_expanded_depth, state.depth);
    //  2本に分かれるが、hashが同じなので1本にまとめられる
    f(Action{}, state.depth + 1, static_cast<Hash>(state.depth + 1));
    f(Action{}, state.depth + 1, static_cast<Hash>(state.depth + 1));
}

}  //  namespace chain

void test_goal_score() {
    auto expand = [](const chain::State& s, auto f) {
        chain::expand(s, f);
    };
    for (bool use_undo: {true, false}) {
        auto run = [&](const BeamSearchConfig& config, double initial_score) {
            chain::max_expanded_depth = -1;
            return use_undo
                       ? beam_search<chain::Action, Hash>(chain::State{0}, initial_score, config, expand, chain::apply,
                                                          chain::undo)
                       : beam_search<chain::Action, Hash>(chain::State{0}, initial_score, config, expand, chain::apply);
        };
        BeamSearchConfig config{100, 10};
        config.goal_score = 5;
        auto result = run(config, 0);
        assert(result.actions.size() == 5 && result.score == 5);
        //  深さ5のノードは展開しない
        assert(chain::max_expanded_depth == 4);

        //  初期状態がゴールなら何もしない
        result = run(config, 5);
        assert(result.actions.empty() && result.score == 5);
        assert(chain::max_expanded_depth == -1);

        //  指定しなければmax_turnまで進む
        result = run(BeamSearchConfig{100, 10}, 0);
        assert(result.actions.size() == 100);
    }
}

//  ---------------- 展開の順序と、時間で幅を決める ----------------
//  4分岐の木。hashは候補ごとに違うので重複排除は起きず、候補数は4倍ずつ増えていく。
//  keyは根からの子番号の列を4進数にしたもの(先頭に1を置く)で、同じ深さならkeyの順がDFS順になる。
//  1ノードの展開にnode_microsecondsかかるようにして、残す候補数が時間で決まることを確かめる

namespace quad {

struct Action {
    int child;
};

struct State {
    int depth;
    unsigned long long key;
};

double node_microseconds;
//  goal_depthに着いたらスコアを大きくする。0ならゴールなし
int goal_depth;
std::vector<int> expanded_per_depth;
//  深さごとに、最後に展開したノードのkey
std::vector<unsigned long long> last_key;
bool dfs_order;

void apply(State& state, const Action& action) {
    state.depth++;
    state.key = state.key * 4 + action.child;
}

void undo(State& state, const Action&) {
    state.depth--;
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
           < node_microseconds) {}
    if (static_cast<int>(expanded_per_depth.size()) <= state.depth) {
        expanded_per_depth.resize(state.depth + 1);
        last_key.resize(state.depth + 1);
    }
    expanded_per_depth[state.depth]++;
    if (last_key[state.depth] >= state.key) dfs_order = false;
    last_key[state.depth] = state.key;
    for (int child = 0; child < 4; child++) {
        unsigned long long key = state.key * 4 + child;
        double score = static_cast<double>(mix(key) % 1000);
        if (goal_depth > 0 && state.depth + 1 == goal_depth) score += 1e6;
        f(Action{child}, score, static_cast<Hash>(mix(key ^ 0x9e3779b97f4a7c15ull)));
    }
}

struct Run {
    BeamSearchResult<Action> result;
    double elapsed;
};

Run run(const BeamSearchConfig& config, bool use_undo) {
    expanded_per_depth.clear();
    last_key.clear();
    dfs_order = true;
    auto e = [](const State& s, auto f) {
        expand(s, f);
    };
    auto start = std::chrono::steady_clock::now();
    auto result = use_undo ? beam_search<Action, Hash>(State{0, 1}, 0, config, e, apply, undo)
                           : beam_search<Action, Hash>(State{0, 1}, 0, config, e, apply);
    return {result, std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count()};
}

}  //  namespace quad

//  各手でfrontierのノードをDFS順に展開すること。
//  undoあり版では、これが崩れるとStateの移動距離が伸びる
void test_dfs_order() {
    quad::node_microseconds = 0;
    quad::goal_depth = 0;
    for (bool use_undo: {true, false}) {
        auto run = quad::run(BeamSearchConfig{30, 50}, use_undo);
        assert(run.result.actions.size() == 30);
        assert(quad::dfs_order);
    }
}

void test_time_limit() {
    quad::node_microseconds = 2;
    for (bool use_undo: {true, false}) {
        quad::goal_depth = 0;
        //  制限時間をほぼ使い切り、それを大きく超えない
        BeamSearchConfig config{100, 20000, 100};
        auto run = quad::run(config, use_undo);
        assert(run.result.actions.size() == 100);
        assert(50 < run.elapsed && run.elapsed < 150);
        //  1手あたり1ミリ秒、1ノード2マイクロ秒なので、幅は数百になる
        int middle = quad::expanded_per_depth[50];
        assert(50 < middle && middle < 5000);

        //  beam_widthを超えない
        config.beam_width = 30;
        run = quad::run(config, use_undo);
        for (int count: quad::expanded_per_depth) assert(count <= 30);
        assert(run.elapsed < 100);

        //  時間を使い切った後は1つずつ残して最後まで進む
        config = BeamSearchConfig{2000, 20000, 1};
        run = quad::run(config, use_undo);
        assert(run.result.actions.size() == 2000);
        assert(quad::expanded_per_depth.back() == 1);
        assert(run.elapsed < 100);
    }
}

void test_expected_turn() {
    quad::node_microseconds = 2;
    for (bool use_undo: {true, false}) {
        quad::goal_depth = 50;
        BeamSearchConfig config{500, 20000, 100};
        config.goal_score = 1e6;
        //  max_turnまで続くとみなすと、50手目でゴールに着いた時点で時間の1割程度しか使っていない
        auto run = quad::run(config, use_undo);
        assert(run.result.actions.size() == 50);
        assert(run.elapsed < 50);
        //  50手と見込めば、時間の大半を使う
        config.expected_turn = 50;
        run = quad::run(config, use_undo);
        assert(run.result.actions.size() == 50);
        assert(50 < run.elapsed && run.elapsed < 150);
    }
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

    //  素朴な実装と答えが一致する(重複排除・終端・打ち切りを含む)
    NaiveCoverage coverage;
    for (unsigned seed = 1; seed <= 6; seed++) {
        for (int width: {1, 3, 20, 200}) {
            test_same_as_naive(seed, 300, width, 30, false, coverage);
            test_same_as_naive(seed, 300, width, 30, true, coverage);
        }
    }
    assert(coverage.stopped_by_goal > 0);
    assert(coverage.answer_at_terminal > 0);
    assert(coverage.answer_at_max_turn > 0);
    test_ties();
    test_goal_score();
    test_dfs_order();
    test_time_limit();
    test_expected_turn();
}

}  //  namespace nono

int main() {
    nono::solve();
}
