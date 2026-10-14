// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/heuristic/search-tree.hpp"

namespace nono {

//  可逆なActionとして「配列のindex番目にdeltaを足す」を使う
struct Action {
    int index, delta;
};
using State = std::vector<int>;

constexpr int WIDTH = 6;

void apply(State& state, const Action& action) {
    state[action.index] += action.delta;
}

void undo(State& state, const Action& action) {
    state[action.index] -= action.delta;
}

//  根からidまでのActionを素直に適用した状態
State reference_state(const SearchTree<Action>& tree, int id) {
    State state(WIDTH, 0);
    for (const Action& action: tree.path(id)) apply(state, action);
    return state;
}

//  親を辿るだけの素朴な実装
std::vector<Action> reference_path(const SearchTree<Action>& tree, int id) {
    std::vector<Action> result;
    while (id != SearchTree<Action>::ROOT) {
        result.push_back(tree.action(id));
        id = tree.parent(id);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

int reference_lca(const SearchTree<Action>& tree, int u, int v) {
    std::vector<int> ancestors;
    for (int x = u;; x = tree.parent(x)) {
        ancestors.push_back(x);
        if (x == SearchTree<Action>::ROOT) break;
    }
    for (int x = v;; x = tree.parent(x)) {
        if (std::find(ancestors.begin(), ancestors.end(), x) != ancestors.end()) return x;
        if (x == SearchTree<Action>::ROOT) break;
    }
    assert(false);
    return SearchTree<Action>::ROOT;
}

void test(unsigned seed, int node_count) {
    std::mt19937 mt(seed);
    SearchTree<Action> tree;
    std::uniform_int_distribution<int> index(0, WIDTH - 1), delta(-9, 9);

    //  根からランダムに木を伸ばす
    for (int i = 1; i < node_count; i++) {
        int parent = std::uniform_int_distribution<int>(0, tree.size() - 1)(mt);
        int id = tree.add(parent, Action{index(mt), delta(mt)});
        assert(id == i);
        assert(tree.parent(id) == parent);
        assert(tree.depth(id) == tree.depth(parent) + 1);
    }
    assert(tree.size() == node_count);
    assert(tree.parent(SearchTree<Action>::ROOT) == -1);
    assert(tree.depth(SearchTree<Action>::ROOT) == 0);
    assert(tree.path(SearchTree<Action>::ROOT).empty());

    //  path()が親を辿るだけの実装と一致し、長さがdepthと一致すること
    for (int id = 0; id < node_count; id++) {
        auto expected = reference_path(tree, id);
        auto actual = tree.path(id);
        assert(actual.size() == expected.size());
        assert((int)actual.size() == tree.depth(id));
        for (std::size_t i = 0; i < actual.size(); i++) {
            assert(actual[i].index == expected[i].index && actual[i].delta == expected[i].delta);
        }
    }

    //  move_state()が、根から辿り直した状態と一致すること。
    //  あわせて、apply/undoの回数が2点間の距離ちょうどであること
    //  (共通祖先まででなく根まで戻る実装になっていれば落ちる)
    int current = SearchTree<Action>::ROOT;
    State state(WIDTH, 0);
    std::uniform_int_distribution<int> node(0, node_count - 1);
    for (int iteration = 0; iteration < 2000; iteration++) {
        int target = node(mt);
        int operations = 0;
        tree.move_state(state, current, target, [&](State& s, const Action& a) {
            operations++;
            apply(s, a);
        }, [&](State& s, const Action& a) {
            operations++;
            undo(s, a);
        });
        int lca = reference_lca(tree, current, target);
        int distance = tree.depth(current) + tree.depth(target) - 2 * tree.depth(lca);
        assert(operations == distance);
        assert(state == reference_state(tree, target));
        current = target;
    }
}

//  一本道の木。fromとtoが祖先子孫関係にある場合
void test_path_graph() {
    SearchTree<Action> tree;
    int previous = SearchTree<Action>::ROOT;
    for (int i = 0; i < 50; i++) previous = tree.add(previous, Action{i % WIDTH, i + 1});

    State state(WIDTH, 0);
    int current = SearchTree<Action>::ROOT;
    auto go = [&](int target) {
        tree.move_state(state, current, target, apply, undo);
        current = target;
        assert(state == reference_state(tree, target));
    };
    go(50);  //  先頭から末尾へ
    go(0);   //  末尾から先頭へ
    go(25);
    go(25);  //  同じノードへの移動は何もしない
    go(50);
}

void solve() {
    test(1, 200);
    test(2, 500);
    test(3, 1000);
    //  ノードが1つ(根のみ)の場合
    test(4, 1);
    test_path_graph();
}

}  //  namespace nono

int main() {
    nono::solve();
}
