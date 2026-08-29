// competitive-verifier: STANDALONE
#include <array>
#include <cassert>
#include <cmath>
#include <random>
#include <vector>

#include "nono/heuristic/selector.hpp"
#include "nono/heuristic/xorshift.hpp"

namespace nono {

constexpr int TRIAL = 1 << 20;

template <int N, class Rng>
std::vector<int> count(const auto& selector, Rng& rng) {
    std::vector<int> counter(N, 0);
    for (int i = 0; i < TRIAL; i++) {
        int index = selector(rng);
        assert(0 <= index && index < N);
        counter[index]++;
    }
    return counter;
}

///  UniformSelectorが[0, N)を一様に返すこと。
///  Nが2冪でない場合も含めて確認する(乗算シフトの丸め)
template <int N, class Rng>
void test_uniform(Rng& rng) {
    auto counter = count<N>(UniformSelector<N>{}, rng);
    double expected = (double)TRIAL / N;
    for (int i = 0; i < N; i++) {
        assert(std::abs(counter[i] - expected) < 0.05 * expected);
    }
}

///  Selectorが重みに比例して返すこと
template <int N, class Rng>
void test_weighted(const std::array<double, N>& weights, Rng& rng) {
    double sum = 0;
    for (double weight: weights) sum += weight;

    auto counter = count<N>(Selector<N>(weights), rng);
    for (int i = 0; i < N; i++) {
        double expected = TRIAL * weights[i] / sum;
        if (weights[i] == 0) {
            //  重み0の要素は決して選ばれない
            assert(counter[i] == 0);
        } else {
            assert(std::abs(counter[i] - expected) < 0.05 * expected);
        }
    }
}

///  デフォルト構築したSelectorが一様であること。
///  以前は重みが未初期化のまま参照されていた
template <int N, class Rng>
void test_default_constructed(Rng& rng) {
    auto counter = count<N>(Selector<N>{}, rng);
    double expected = (double)TRIAL / N;
    for (int i = 0; i < N; i++) {
        assert(std::abs(counter[i] - expected) < 0.05 * expected);
    }
}

template <class Rng>
void test_all(Rng& rng) {
    test_uniform<1>(rng);
    test_uniform<2>(rng);
    test_uniform<3>(rng);
    test_uniform<5>(rng);
    test_uniform<8>(rng);
    test_uniform<17>(rng);

    test_default_constructed<1>(rng);
    test_default_constructed<3>(rng);
    test_default_constructed<8>(rng);

    test_weighted<1>({1.0}, rng);
    test_weighted<2>({1.0, 1.0}, rng);
    test_weighted<2>({3.0, 1.0}, rng);
    test_weighted<3>({1.0, 0.0, 3.0}, rng);
    test_weighted<4>({0.1, 0.2, 0.3, 0.4}, rng);
    //  スケールが変わっても比だけで決まる
    test_weighted<3>({100.0, 0.0, 300.0}, rng);
    test_weighted<3>({1e-3, 0.0, 3e-3}, rng);
}

void solve() {
    static_assert(SelectionStrategy<UniformSelector<4>, Xorshift<unsigned>>);
    static_assert(SelectionStrategy<Selector<4>, Xorshift<unsigned>>);
    static_assert(SelectionStrategy<UniformSelector<4>, std::mt19937_64>);
    static_assert(SelectionStrategy<Selector<4>, std::mt19937_64>);

    std::mt19937 mt(12345);
    test_all(mt);
    Xorshift<unsigned> xorshift(12345);
    test_all(xorshift);
    //  64bitのRngでも同じ結果になること
    std::mt19937_64 mt64(12345);
    test_all(mt64);
}

}  //  namespace nono

int main() {
    nono::solve();
}
