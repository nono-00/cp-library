// competitive-verifier: STANDALONE
#include <cassert>
#include <set>

#include "nono/heuristic/timer.hpp"

namespace nono {

//  progressが単調非減少で、制限時間を超えた時点でexpiredが立つこと
void test_progress() {
    constexpr double LIMIT = 100;
    Timer timer(LIMIT);
    assert(!timer.expired());
    assert(timer.progress() >= 0);

    double previous = timer.progress();
    while (!timer.expired()) {
        double progress = timer.progress();
        assert(progress >= previous);
        previous = progress;
    }
    //  詰まったループで監視しているので、超過幅はごく小さいはず
    double elapsed = timer.elapsed();
    assert(LIMIT <= elapsed && elapsed < LIMIT * 3);
    assert(timer.progress() > 1.0);
}

//  progressがミリ秒に量子化されていないこと。
//  以前はduration_cast<milliseconds>で切り捨てていたため、
//  progressが階段状になり、焼きなましの温度もその粒度でしか動かなかった
void test_sub_millisecond_resolution() {
    Timer timer(1000);
    std::set<double> values;
    //  5ミリ秒の間に観測される相異なるprogressの個数を数える。
    //  ミリ秒粒度なら高々6個程度にしかならない
    while (timer.elapsed() < 5.0) {
        values.insert(timer.progress());
    }
    assert(values.size() > 100);
}

//  経過時間が制限時間の指定に依存しないこと
void test_elapsed_is_independent_of_limit() {
    Timer fast(1);
    Timer slow(1000000);
    while (fast.elapsed() < 20.0) {}
    double difference = fast.elapsed() - slow.elapsed();
    assert(-1.0 < difference && difference < 1.0);
    //  progressは制限時間で割った値なので大きく異なる
    assert(fast.progress() > slow.progress() * 1000);
}

void solve() {
    test_progress();
    test_sub_millisecond_resolution();
    test_elapsed_is_independent_of_limit();
}

}  //  namespace nono

int main() {
    nono::solve();
}
