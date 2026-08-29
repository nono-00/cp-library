// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

#include "nono/heuristic/annealer.hpp"
#include "nono/heuristic/xorshift.hpp"

namespace nono {

//  焼きなましは時間駆動なので実行ごとの再現性がない。
//  そのため「同じ結果になること」ではなく、以下の不変条件を検証する。
//  [1] accept()の受理率が理論値exp(delta / T)に一致すること
//  [2] 差分で積み上げたbest_score()が、ベスト状態を再計算したスコアと一致すること
//  [3] 最適解が既知の小さな問題で最適解に到達すること

///  [1] 受理率が理論値と一致するか
void test_accept_probability() {
    constexpr int TRIAL = 1 << 20;

    auto rate = [](double temperature, double delta) {
        Xorshift<unsigned> rng(12345);
        int state = 0;
        Annealer sa(state, 0.0, {.time_limit = 1e9, .start_temp = temperature, .end_temp = temperature}, rng);
        int accepted = 0;
        for (int i = 0; i < TRIAL; i++) accepted += sa.accept(delta);
        return (double)accepted / TRIAL;
    };

    //  改善方向は常に受理する
    assert(rate(1.0, 0.0) == 1.0);
    assert(rate(1.0, 1.0) == 1.0);
    //  温度が十分高ければほぼ全て受理、十分低ければ全く受理しない
    assert(rate(1e9, -1.0) > 0.99);
    assert(rate(1e-9, -1.0) == 0.0);
    //  中間: exp(-1) = 0.3679, exp(-2) = 0.1353, exp(-0.5) = 0.6065
    assert(std::abs(rate(1.0, -1.0) - std::exp(-1.0)) < 0.02);
    assert(std::abs(rate(1.0, -2.0) - std::exp(-2.0)) < 0.02);
    assert(std::abs(rate(2.0, -1.0) - std::exp(-0.5)) < 0.02);
    //  deltaと温度は比でしか効かない
    assert(std::abs(rate(10.0, -10.0) - rate(1.0, -1.0)) < 0.02);
}

///  巡回セールスマン問題。最大化に揃えるためスコアは経路長の符号反転
struct Tsp {
    int n;
    std::vector<std::vector<double>> dist;

    explicit Tsp(int n, unsigned seed): n(n), dist(n, std::vector<double>(n)) {
        std::mt19937 mt(seed);
        std::uniform_real_distribution<double> uniform(0.0, 100.0);
        std::vector<double> x(n), y(n);
        for (int i = 0; i < n; i++) x[i] = uniform(mt), y[i] = uniform(mt);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) dist[i][j] = std::hypot(x[i] - x[j], y[i] - y[j]);
    }

    double score(const std::vector<int>& tour) const {
        double sum = 0;
        for (int i = 0; i < n; i++) sum += dist[tour[i]][tour[(i + 1) % n]];
        return -sum;
    }

    ///  [i + 1, j]を反転したときのスコア差分
    double delta(const std::vector<int>& tour, int i, int j) const {
        int a = tour[i], b = tour[(i + 1) % n], c = tour[j], d = tour[(j + 1) % n];
        return -(dist[a][c] + dist[b][d] - dist[a][b] - dist[c][d]);
    }

    double brute_force() const {
        std::vector<int> tour(n);
        std::iota(tour.begin(), tour.end(), 0);
        double best = score(tour);
        //  始点を固定して残りを全探索
        while (std::next_permutation(tour.begin() + 1, tour.end())) {
            best = std::max(best, score(tour));
        }
        return best;
    }
};

template <class BestPolicy, bool Logging>
void test_tsp(unsigned seed) {
    constexpr int N = 8;
    Tsp tsp(N, seed);
    double optimal = tsp.brute_force();

    std::vector<int> tour(N);
    std::iota(tour.begin(), tour.end(), 0);
    Xorshift<unsigned> rng(seed);

    Annealer<std::vector<int>, Xorshift<unsigned>, BestPolicy, Logging> sa(
        tour, tsp.score(tour), {.time_limit = 100, .start_temp = 30, .end_temp = 0.01}, rng);
    sa.name(0, "2opt");

    while (sa.running()) {
        int i = rng() % N, j = rng() % N;
        if (i > j) std::swap(i, j);
        if (i == j) continue;
        double delta = tsp.delta(tour, i, j);
        sa.try_move(delta, [&] { std::reverse(tour.begin() + i + 1, tour.begin() + j + 1); }, 0);
    }

    assert(sa.iteration() > 1000);
    //  [2] 差分の積み上げが正しいこと
    assert(std::abs(tsp.score(tour) - sa.score()) < 1e-6);
    if constexpr (std::is_same_v<BestPolicy, CopyBest>) {
        assert(std::abs(tsp.score(sa.best()) - sa.best_score()) < 1e-6);
    }
    //  [3] 最適解に到達していること
    assert(sa.best_score() > optimal - 1e-6);
    assert(sa.best_score() < optimal + 1e-6);

    //  dump()がどの構成でもコンパイル・実行できること
    std::ostringstream oss;
    sa.dump(oss);
    assert(!oss.str().empty());
}

///  accept / commitを直接使う経路
void test_manual_commit() {
    constexpr int N = 8;
    Tsp tsp(N, 7);
    double optimal = tsp.brute_force();

    std::vector<int> tour(N);
    std::iota(tour.begin(), tour.end(), 0);
    Xorshift<unsigned> rng(7);
    Annealer sa(tour, tsp.score(tour), {.time_limit = 100, .start_temp = 30, .end_temp = 0.01}, rng);

    long long improved = 0;
    while (sa.running()) {
        int i = rng() % N, j = rng() % N;
        if (i > j) std::swap(i, j);
        if (i == j) continue;
        double delta = tsp.delta(tour, i, j);
        bool accepted = sa.accept(delta);
        if (accepted) {
            std::reverse(tour.begin() + i + 1, tour.begin() + j + 1);
            sa.commit(delta);
            improved += sa.improved();
        }
        sa.record(delta, accepted, 0);
        sa.audit(tsp.score(tour));
    }
    assert(improved > 0);
    assert(std::abs(tsp.score(sa.best()) - sa.best_score()) < 1e-6);
    assert(sa.best_score() > optimal - 1e-6);
}

///  progressとtemperatureが単調に進むこと
void test_schedule() {
    int state = 0;
    Xorshift<unsigned> rng(1);
    Annealer sa(state, 0.0, {.time_limit = 50, .start_temp = 100, .end_temp = 1}, rng);

    double prev_progress = 0, prev_temperature = 100;
    int checks = 0;
    while (sa.running()) {
        double progress = sa.progress();
        double temperature = sa.temperature();
        assert(0 <= progress && progress < 1);
        assert(progress >= prev_progress);
        assert(temperature <= prev_temperature + 1e-9);
        assert(1 - 1e-6 <= temperature && temperature <= 100 + 1e-6);
        if (progress > prev_progress) checks++;
        prev_progress = progress;
        prev_temperature = temperature;
    }
    //  時間取得の間引きが働いていても、温度は十分細かく更新される
    assert(checks > 100);
    assert(prev_progress > 0.9);
}

void solve() {
    test_accept_probability();
    test_schedule();
    for (unsigned seed: {1u, 2u, 3u}) {
        test_tsp<CopyBest, false>(seed);
        test_tsp<CopyBest, true>(seed);
        test_tsp<NoBestState, true>(seed);
    }
    test_manual_commit();
}

}  //  namespace nono

int main() {
    nono::solve();
}
