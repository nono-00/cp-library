// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

#include "nono/heuristic/annealer.hpp"
#include "nono/heuristic/move-log.hpp"
#include "nono/heuristic/xorshift.hpp"

namespace nono {

//  N個のアイテムをK個の箱に配り、負荷の二乗和を最小化する問題。
//  差分評価がO(1)でStateが重いため、ベストのフルコピーが効いてしまう典型例
namespace bin {

constexpr int N = 200;
constexpr int K = 7;

struct State {
    std::vector<int> assign;
    std::vector<long long> load;
    bool operator==(const State&) const = default;
};

struct Move {
    int item, from, to;
};

std::vector<long long> weight;

State initial() {
    State state;
    state.assign.assign(N, 0);
    state.load.assign(K, 0);
    for (int i = 0; i < N; i++) state.load[0] += weight[i];
    return state;
}

double score(const State& state) {
    double sum = 0;
    for (auto load: state.load) sum += (double)load * load;
    return -sum;
}

double delta(const State& state, const Move& move) {
    long long from = state.load[move.from], to = state.load[move.to], w = weight[move.item];
    return -((double)(from - w) * (from - w) + (double)(to + w) * (to + w) - (double)from * from - (double)to * to);
}

void apply(State& state, const Move& move) {
    state.assign[move.item] = move.to;
    state.load[move.from] -= weight[move.item];
    state.load[move.to] += weight[move.item];
}

void undo(State& state, const Move& move) {
    state.assign[move.item] = move.from;
    state.load[move.to] -= weight[move.item];
    state.load[move.from] += weight[move.item];
}

}  //  namespace bin

///  Annealerを介さず固定回数で回すことで、完全に再現性のある比較を行う。
///  「ベストをフルコピーで持った場合」と「MoveLogで持った場合」が
///  厳密に同じ状態・同じスコアを返すことを確認する
void test_against_full_copy(std::size_t capacity, int iterations, unsigned seed) {
    std::mt19937 mt(seed);
    bin::State state = bin::initial();
    double current = bin::score(state);

    //  素朴なフルコピー版
    bin::State naive_best = state;
    double naive_best_score = current;

    auto log = make_move_log<bin::Move>(state, [](bin::State& s, const bin::Move& m) { bin::undo(s, m); }, capacity);

    std::uniform_int_distribution<int> item(0, bin::N - 1), box(0, bin::K - 1);
    for (int i = 0; i < iterations; i++) {
        bin::Move move{item(mt), 0, box(mt)};
        move.from = state.assign[move.item];
        if (move.from == move.to) continue;
        double d = bin::delta(state, move);
        //  受理判定は焼きなましと無関係な固定のコイン。再現性のため
        if (d < 0 && mt() % 4 != 0) continue;

        bin::apply(state, move);
        current += d;
        log.push(move);
        if (current > naive_best_score) {
            naive_best_score = current;
            naive_best = state;
            log.mark();
        }
    }

    const bin::State& restored = log.restore();
    assert(restored == naive_best);
    assert(std::abs(bin::score(restored) - naive_best_score) < 1e-6);
    //  restore()は冪等
    const bin::State& again = log.restore();
    assert(again == naive_best);
}

///  Annealerと組み合わせた経路。MoveLog経由のベストが、
///  Annealerが追跡しているベストスコアと一致すること
void test_with_annealer(unsigned seed) {
    bin::State state = bin::initial();
    Xorshift<unsigned> rng(seed);

    Annealer<bin::State, Xorshift<unsigned>, NoBestState> sa(
        state, bin::score(state), {.time_limit = 50, .start_temp = 1e7, .end_temp = 1e3}, rng);
    auto log = make_move_log<bin::Move>(state, [](bin::State& s, const bin::Move& m) { bin::undo(s, m); }, 1024);

    while (sa.running()) {
        bin::Move move{(int)(rng() % bin::N), 0, (int)(rng() % bin::K)};
        move.from = state.assign[move.item];
        if (move.from == move.to) continue;
        double d = bin::delta(state, move);
        if (!sa.accept(d)) continue;
        bin::apply(state, move);
        log.commit(sa, d, move);
    }

    assert(sa.iteration() > 1000);
    assert(log.compactions() > 0);
    assert(sa.best_score() > bin::score(bin::initial()));

    const bin::State& best = log.restore();
    //  差分で積み上げたベストスコアと、復元した状態の再計算値が一致する
    assert(std::abs(bin::score(best) - sa.best_score()) < 1e-6);
    //  loadがassignと整合している(undoが正しく効いている)
    std::vector<long long> load(bin::K, 0);
    for (int i = 0; i < bin::N; i++) load[best.assign[i]] += bin::weight[i];
    assert(load == best.load);

    std::ostringstream oss;
    log.dump(oss);
    assert(!oss.str().empty());
}

///  ベストが一度も更新されない場合、restore()は初期状態を返す
void test_never_improved() {
    bin::State state = bin::initial();
    auto log = make_move_log<bin::Move>(state, [](bin::State& s, const bin::Move& m) { bin::undo(s, m); }, 4);
    bin::State expected = state;

    std::mt19937 mt(1);
    std::uniform_int_distribution<int> item(0, bin::N - 1), box(0, bin::K - 1);
    for (int i = 0; i < 1000; i++) {
        bin::Move move{item(mt), 0, box(mt)};
        move.from = state.assign[move.item];
        if (move.from == move.to) continue;
        bin::apply(state, move);
        log.push(move);
    }
    assert(state != expected);
    assert(log.restore() == expected);
}

void solve() {
    std::mt19937 mt(42);
    bin::weight.resize(bin::N);
    for (int i = 0; i < bin::N; i++) bin::weight[i] = mt() % 1000 + 1;

    //  capacityを変えてcompactionの発生頻度を変える。1は毎回compaction
    for (std::size_t capacity:
         {std::size_t(1), std::size_t(2), std::size_t(7), std::size_t(64), std::size_t(1000000)}) {
        for (unsigned seed: {1u, 2u, 3u}) {
            test_against_full_copy(capacity, 3000, seed);
        }
    }
    test_never_improved();
    for (unsigned seed: {1u, 2u}) test_with_annealer(seed);
}

}  //  namespace nono

int main() {
    nono::solve();
}
