#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

#include "nono/utility/is-debug.hpp"

namespace nono {

///  # ベスト解の保持方針
///  CopyBest    : ベスト更新のたびにStateをコピーする(既定)
///  NoBestState : ベストスコアのみ追跡し、状態は保持しない
///                Stateのコピーが重い場合に、move-log.hppのMoveLogと組み合わせて使う
struct CopyBest {};
struct NoBestState {};

namespace annealer_internal {

///  受理判定 exp(delta / T) > u を delta * (1 / T) > log(u) に変形して使う。
///  log(u)を引くためのテーブル。log_table[i] = log((i + 0.5) / LOG_TABLE_SIZE) (負値)
///
///  サイズは意図的に小さくしている。大きくするとランダムアクセスでL1dを汚す一方、
///  得られるのは「exp(delta / T)が極端に小さい遷移も稀に受理する」という
///  焼きなましの挙動にほぼ影響しない精度でしかない。
///  1 << 10 なら u >= 5e-4 相当、すなわち delta / T < -7.6 の遷移が
///  「確率5e-4で受理」から「受理しない」に変わるだけ。
constexpr int LOG_TABLE_BITS = 10;
constexpr int LOG_TABLE_SIZE = 1 << LOG_TABLE_BITS;
constexpr int LOG_TABLE_MASK = LOG_TABLE_SIZE - 1;

///  近傍種別の上限。統計を固定長配列で持つために必要
constexpr int MAX_KIND = 8;

inline const float* log_table() {
    static const std::array<float, LOG_TABLE_SIZE> table = [] {
        std::array<float, LOG_TABLE_SIZE> result{};
        for (int i = 0; i < LOG_TABLE_SIZE; i++) {
            result[i] = static_cast<float>(std::log((i + 0.5) / LOG_TABLE_SIZE));
        }
        return result;
    }();
    return table.data();
}

inline double now_ms() {
    using namespace std::chrono;
    return duration<double, std::milli>(steady_clock::now().time_since_epoch()).count();
}

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

struct Stat {
    long long calls = 0;
    long long accepts = 0;
    long long improves = 0;
    double sum_delta = 0;
    double min_delta = std::numeric_limits<double>::max();
    double max_delta = std::numeric_limits<double>::lowest();
};

///  Logging = falseのときにメモリも命令も消す
template <bool Logging>
struct LogStorage {
    std::array<Stat, MAX_KIND> stats{};
    std::array<std::string_view, MAX_KIND> names{};
    long long best_updates = 0;
    double best_update_ms = 0;
    double best_progress = 0;
};

template <>
struct LogStorage<false> {};

///  ベスト解の保持。方針ごとに特殊化する
template <class State, class Policy>
struct BestStorage;

template <class State>
struct BestStorage<State, CopyBest> {
    State value;
    explicit BestStorage(const State& state): value(state) {}
    void update(const State& state) {
        value = state;
    }
};

template <class State>
struct BestStorage<State, NoBestState> {
    explicit BestStorage(const State&) {}
    void update(const State&) {}
};

}  //  namespace annealer_internal

struct AnnealerConfig {
    ///  ミリ秒。1秒なら1000
    double time_limit;
    ///  開始温度。「序盤に許容したい悪化幅」と同じ単位で考えるとよい
    double start_temp;
    ///  終了温度。0より大きい必要がある
    double end_temp;
};

///  # Annealer
///  焼きなましのループ駆動部分だけを担う。近傍の型には一切関与しない。
///  提供するのは「時間・温度・受理判定・スコア追跡・ベスト解保持・統計」のみで、
///  状態表現・近傍・差分評価は利用者が素のコードとして書く。
///
///  ## 使い方
///  ```
///  nono::Annealer sa(state, eval(state), {.time_limit = 1900, .start_temp = 100, .end_temp = 0.5}, rng);
///  while (sa.running()) {
///      auto move = generate(state, rng);
///      double delta = evaluate_delta(state, move);
///      if (sa.accept(delta)) {
///          apply(state, move);
///          sa.commit(delta);
///      }
///  }
///  state = sa.release_best();
///  ```
///  apply -> commitの順序が必要なので、accept/commitは分かれている。
///  順序を間違えたくない場合はtry_move(delta, apply)を使う。
///
///  ## 最大化
///  スコアは最大化として扱う。最小化したい場合は符号を反転して渡す。
///
///  ## 注意
///  提案分布の対称性(詳細釣り合い)は保証しない。非対称な近傍を混ぜた場合の
///  挙動は利用者の責任。
template <class State, std::uniform_random_bit_generator Rng, class BestPolicy = CopyBest,
          bool Logging = nono::is_debug()>
class Annealer {
    static_assert(std::is_same_v<BestPolicy, CopyBest> || std::is_same_v<BestPolicy, NoBestState>);
    static constexpr int RNG_BITS = annealer_internal::rng_bits<Rng>();
    static_assert(RNG_BITS >= annealer_internal::LOG_TABLE_BITS, "Rngの値域が狭すぎる");
    static constexpr int RNG_SHIFT = RNG_BITS - annealer_internal::LOG_TABLE_BITS;

  public:
    ///  stateとrngは参照として保持する。Annealerより長生きしなければならない
    Annealer(State& state, double score, const AnnealerConfig& config, Rng& rng)
        : state_(std::addressof(state)),
          rng_(std::addressof(rng)),
          best_(state),
          log_table_(annealer_internal::log_table()),
          score_(score),
          best_score_(score),
          initial_score_(score),
          time_limit_(config.time_limit),
          log_start_temp_(std::log(config.start_temp)),
          log_end_temp_(std::log(config.end_temp)),
          start_ms_(annealer_internal::now_ms()),
          last_ms_(start_ms_),
          inv_temp_(std::exp(-log_start_temp_)) {
        assert(config.start_temp > 0 && config.end_temp > 0);
        assert(config.time_limit > 0);
    }

    ///  # running()
    ///  ループ条件。時間取得は間引かれるので毎回呼んでよい
    bool running() {
        iteration_++;
        if (--countdown_ > 0) [[likely]]
            return true;
        return refresh_();
    }

    ///  # accept(delta)
    ///  受理判定のみを行う。状態もスコアも変更しない
    bool accept(double delta) {
        if (delta >= 0) return true;
        auto value = static_cast<unsigned long long>((*rng_)() - Rng::min());
        return delta * inv_temp_ > log_table_[(value >> RNG_SHIFT) & annealer_internal::LOG_TABLE_MASK];
    }

    ///  # commit(delta)
    ///  状態を変更した後に呼ぶ。スコアを更新し、必要ならベスト解を保存する
    void commit(double delta) {
        score_ += delta;
        improved_ = score_ > best_score_;
        if (improved_) {
            best_score_ = score_;
            if constexpr (Logging && std::is_same_v<BestPolicy, CopyBest>) {
                double begin = annealer_internal::now_ms();
                best_.update(*state_);
                log_.best_update_ms += annealer_internal::now_ms() - begin;
            } else {
                best_.update(*state_);
            }
            if constexpr (Logging) {
                log_.best_updates++;
                log_.best_progress = progress_;
            }
        }
    }

    ///  # try_move(delta, apply, kind)
    ///  accept -> apply -> commitを融合したもの。applyは引数なしの呼び出し可能物
    template <class F>
    bool try_move(double delta, F&& apply, int kind = 0) {
        if constexpr (Logging) record_call_(delta, kind);
        if (!accept(delta)) return false;
        if constexpr (Logging) record_accept_(delta, kind);
        std::forward<F>(apply)();
        commit(delta);
        return true;
    }

    ///  # record(delta, kind)
    ///  accept/commitを直接使う場合に、統計だけを別途記録したいとき用
    void record(double delta, bool accepted, int kind) {
        if constexpr (Logging) {
            record_call_(delta, kind);
            if (accepted) record_accept_(delta, kind);
        }
    }

    ///  # improved()
    ///  直前のcommit()でベストが更新されたか。MoveLogと組み合わせるときに使う
    bool improved() const {
        return improved_;
    }

    double score() const {
        return score_;
    }
    double best_score() const {
        return best_score_;
    }
    double temperature() const {
        return 1.0 / inv_temp_;
    }
    ///  直近の時間計測時点での進捗。ブロック内では更新されない
    double progress() const {
        return progress_;
    }
    long long iteration() const {
        return iteration_;
    }
    Rng& rng() {
        return *rng_;
    }

    const State& best() const
        requires std::is_same_v<BestPolicy, CopyBest>
    {
        return best_.value;
    }
    State release_best()
        requires std::is_same_v<BestPolicy, CopyBest>
    {
        return std::move(best_.value);
    }

    ///  # audit(recomputed)
    ///  差分評価で積み上げたスコアと、全計算し直したスコアの一致を確認する。
    ///  deltaの加算を繰り返すと浮動小数点誤差が蓄積するため、
    ///  手元では定期的に呼んでおくとよい
    void audit(double recomputed) const {
        DEBUG_ASSERT(std::abs(recomputed - score_) <= 1e-6 * std::max(1.0, std::abs(recomputed)));
    }

    ///  # name(kind, name)
    ///  ログ出力用に近傍種別へ名前を付ける
    void name(int kind, std::string_view name) {
        if constexpr (Logging) {
            assert(0 <= kind && kind < annealer_internal::MAX_KIND);
            log_.names[kind] = name;
        }
    }

    void dump(std::ostream& os = std::cerr) const {
        double elapsed = annealer_internal::now_ms() - start_ms_;
        os << "# Annealer\n";
        os << "  score       : " << initial_score_ << " -> " << best_score_ << '\n';
        os << "  iterations  : " << iteration_ << "  (" << std::fixed << std::setprecision(2)
           << (elapsed > 0 ? iteration_ / elapsed / 1000.0 : 0.0) << " M iter/s)\n";
        os << "  elapsed     : " << elapsed << " ms\n";
        if constexpr (!Logging) {
            os << "  (Loggingが無効なため統計は出力されない)\n";
            return;
        } else {
            os << "  best update : " << log_.best_updates << " times, last at " << (100.0 * log_.best_progress)
               << "%\n";
            if constexpr (std::is_same_v<BestPolicy, CopyBest>) {
                //  ここが数%を超えるなら、NoBestState + MoveLogへの切り替えを検討する
                os << "  best copy   : " << log_.best_update_ms << " ms ("
                   << (elapsed > 0 ? 100.0 * log_.best_update_ms / elapsed : 0.0) << "% of elapsed)\n";
            }
            os << "  " << std::left << std::setw(12) << "kind" << std::right << std::setw(14) << "calls"
               << std::setw(14) << "accepts" << std::setw(9) << "acc%" << std::setw(9) << "imp%" << std::setw(14)
               << "avg delta" << std::setw(14) << "min delta" << std::setw(14) << "max delta" << '\n';
            for (int kind = 0; kind < annealer_internal::MAX_KIND; kind++) {
                const auto& stat = log_.stats[kind];
                if (stat.calls == 0) continue;
                os << "  " << std::left << std::setw(12)
                   << (log_.names[kind].empty() ? std::string_view("?") : log_.names[kind]) << std::right
                   << std::setw(14) << stat.calls << std::setw(14) << stat.accepts << std::setw(9)
                   << (100.0 * stat.accepts / stat.calls) << std::setw(9) << (100.0 * stat.improves / stat.calls)
                   << std::setw(14) << (stat.sum_delta / stat.calls) << std::setw(14) << stat.min_delta << std::setw(14)
                   << stat.max_delta << '\n';
            }
        }
    }

  private:
    bool refresh_() {
        double now = annealer_internal::now_ms();
        progress_ = (now - start_ms_) / time_limit_;
        if (progress_ >= 1.0) return false;

        //  除算を毎イテレーションに残さないため、逆数の形で温度を持つ
        inv_temp_ = std::exp(-(log_start_temp_ + (log_end_temp_ - log_start_temp_) * progress_));

        //  1ブロックがTARGET_MS程度になるようブロック長を実測から調整する。
        //  これにより、1イテレーションの重さに関わらず
        //  「時間取得のコスト」と「温度の階段化」の両方が無視できる水準に収まる
        constexpr double TARGET_MS = 0.1;
        double duration = now - last_ms_;
        long long iterations = iteration_ - last_iteration_;
        if (duration <= 0) {
            //  時計の分解能が粗い環境
            block_ = std::min<long long>(block_ * 2, MAX_BLOCK);
        } else {
            block_ = std::clamp<long long>(static_cast<long long>(iterations * (TARGET_MS / duration)), MIN_BLOCK,
                                           MAX_BLOCK);
        }
        last_ms_ = now;
        last_iteration_ = iteration_;
        countdown_ = block_;
        return true;
    }

    void record_call_(double delta, int kind) {
        assert(0 <= kind && kind < annealer_internal::MAX_KIND);
        auto& stat = log_.stats[kind];
        stat.calls++;
        stat.sum_delta += delta;
        stat.min_delta = std::min(stat.min_delta, delta);
        stat.max_delta = std::max(stat.max_delta, delta);
    }

    void record_accept_(double delta, int kind) {
        auto& stat = log_.stats[kind];
        stat.accepts++;
        if (delta >= 0) stat.improves++;
    }

    static constexpr long long MIN_BLOCK = 16;
    static constexpr long long MAX_BLOCK = 1 << 20;

    State* state_;
    Rng* rng_;
    [[no_unique_address]] annealer_internal::BestStorage<State, BestPolicy> best_;
    const float* log_table_;

    double score_, best_score_, initial_score_;
    double time_limit_, log_start_temp_, log_end_temp_;
    double start_ms_, last_ms_;
    double inv_temp_;
    double progress_ = 0.0;

    long long iteration_ = 0, last_iteration_ = 0;
    long long block_ = MIN_BLOCK, countdown_ = MIN_BLOCK;
    bool improved_ = false;

    [[no_unique_address]] annealer_internal::LogStorage<Logging> log_;
};

}  //  namespace nono
