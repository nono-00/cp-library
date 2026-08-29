#pragma once

#include <array>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <random>

namespace nono {

template <class Selector, class Rng = std::mt19937>
concept SelectionStrategy = std::uniform_random_bit_generator<Rng> && requires(const Selector selector, Rng& rng) {
    { selector(rng) } -> std::convertible_to<int>;
};

namespace selector_internal {

///  Rngが返す値の有効ビット幅
template <class Rng>
constexpr int rng_bits() {
    auto range = Rng::max() - Rng::min();
    int bits = 0;
    while (range > 0) {
        range >>= 1;
        bits++;
    }
    return bits;
}

///  Rngの出力を[0, 2^32)へ均等に写す。
///  値域が2^k - 1でないRngは扱わない(偏りが出るため)
template <std::uniform_random_bit_generator Rng>
inline std::uint64_t sample32(Rng& rng) {
    constexpr int BITS = rng_bits<Rng>();
    static_assert(16 <= BITS && BITS <= 64, "Rngの値域が狭すぎる/広すぎる");
    static_assert(Rng::max() - Rng::min() == (BITS == 64 ? ~std::uint64_t(0) : (std::uint64_t(1) << BITS) - 1),
                  "値域が2^k - 1でないRngには対応していない");
    std::uint64_t value = static_cast<std::uint64_t>(rng() - Rng::min());
    if constexpr (BITS > 32) {
        return value >> (BITS - 32);
    } else {
        return value << (32 - BITS);
    }
}

///  重みを[0, 2^32)の整数に写すためのスケール
constexpr std::uint64_t SCALE = std::uint64_t(1) << 32;

}  //  namespace selector_internal

///  # UniformSelector<N>
///  [0, N)を一様に返す
template <int N>
class UniformSelector {
    static_assert(N > 0);

  public:
    ///  # operator()(rng)
    ///  O(1)
    template <std::uniform_random_bit_generator Rng = std::mt19937>
    inline int operator()(Rng& rng) const {
        //  剰余は除算命令を伴い、焼きなましのように毎イテレーション呼ぶ箇所では
        //  無視できないコストになる。上位ビットを使った乗算シフトで置き換える
        return static_cast<int>((selector_internal::sample32(rng) * N) >> 32);
    }
};

static_assert(SelectionStrategy<UniformSelector<4>>);

///  # Selector<N>
///  重み付きで[0, N)を返す。重み0の要素は選ばれない
template <int N>
class Selector {
    static_assert(N > 0);

  public:
    ///  # Selector()
    ///  一様
    Selector() {
        for (int i = 0; i < N; i++) {
            threshold_[i] = static_cast<std::uint64_t>(selector_internal::SCALE * (i + 1) / N);
        }
    }

    ///  # Selector(weights)
    ///  weightsは非負で、総和が正であること
    explicit Selector(const std::array<double, N>& weights) {
        double sum = 0;
        for (double weight: weights) {
            assert(weight >= 0);
            sum += weight;
        }
        assert(sum > 0);
        //  累積重みを[0, 2^32]の整数に写しておき、抽選を整数比較だけで済ませる。
        //  uniform_real_distributionをホットパスから外すのが狙い
        double accumulated = 0;
        for (int i = 0; i < N; i++) {
            accumulated += weights[i];
            threshold_[i] = static_cast<std::uint64_t>(accumulated / sum * selector_internal::SCALE);
        }
    }

    ///  # operator()(rng)
    ///  O(N)
    template <std::uniform_random_bit_generator Rng = std::mt19937>
    inline int operator()(Rng& rng) const {
        std::uint64_t value = selector_internal::sample32(rng);
        //  最後の要素は閾値の丸め誤差に関わらず選ばれるよう、比較から外す
        for (int i = 0; i < N - 1; i++) {
            if (value < threshold_[i]) return i;
        }
        return N - 1;
    }

  private:
    ///  threshold_[i] = 重みの累積和(0以上i以下)を[0, 2^32]に写した値
    std::array<std::uint64_t, N> threshold_{};
};

static_assert(SelectionStrategy<Selector<4>>);

}  //  namespace nono
