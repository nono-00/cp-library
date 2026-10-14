#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

namespace nono {

//  # MoveLog
//  焼きなましのベスト解を、Stateのコピーではなく「受理した遷移の履歴」として保持する。
//
//  ## 何のためにあるか
//  ベスト更新のたびにStateをまるごとコピーする方式は、Stateが軽ければ無視できるが、
//  「差分評価がO(1)」かつ「Stateが大きい」問題では実行時間の大半を食い潰す。
//  MoveLogは受理のたびにMoveを1個積むだけなので、ベスト更新の頻度にも
//  Stateの大きさにも依存しない。復元は終了時に1回だけ行う。
//
//  ## 要件
//  Moveが可逆であること。すなわちundo(state, move)でapplyを打ち消せること。
//  「区間をまるごと再計算する」ような逆操作を書けない近傍には使えない。
//  その場合は素直にnono::CopyBestを使う。
//
//  ## 使い方
//  ```
//  nono::Annealer<State, Rng, nono::NoBestState> sa(state, score, config, rng);
//  auto log = nono::make_move_log<Move>(state, [](State& s, const Move& m) { undo(s, m); });
//  while (sa.running()) {
//      Move move = generate(state, rng);
//      double delta = evaluate_delta(state, move);
//      if (sa.accept(delta)) {
//          apply(state, move);
//          log.commit(sa, delta, move);  //  sa.commit(delta) + 履歴への記録
//      }
//  }
//  const State& best = log.restore();  //  stateがベスト状態まで巻き戻される
//  ```
//
//  ## メモリ
//  capacity * sizeof(Move)バイトを確保する。履歴がcapacityに達した時点で、
//  ベスト状態を実体化してから履歴を捨てる(compaction)。
//  1回のcompactionはStateのコピー1回 + 最大capacity回のundoで済む。
template <class Move, class State, class Undo>
class MoveLog {
  public:
    //  # DEFAULT_CAPACITY
    //  capacityの既定値
    static constexpr std::size_t DEFAULT_CAPACITY = 1 << 20;

    //  # MoveLog(state, undo, capacity)
    //  undoは undo(state, move) の形で、moveの適用を打ち消す関数。capacityは履歴に積めるMoveの個数
    //  stateは参照として保持する。MoveLogより長生きしないと壊れる
    //  capacity > 0 でないと壊れる
    MoveLog(State& state, Undo undo, std::size_t capacity = DEFAULT_CAPACITY)
        : state_(std::addressof(state)),
          best_(state),
          undo_(std::move(undo)),
          capacity_(capacity) {
        assert(capacity > 0);
        log_.reserve(capacity);
    }

    //  # commit(annealer, delta, move)
    //  applyの後に呼ぶ。annealer.commit(delta)と履歴への記録をまとめて行う
    template <class Annealer>
    void commit(Annealer& annealer, double delta, const Move& move) {
        push(move);
        annealer.commit(delta);
        if (annealer.improved()) mark();
    }

    //  # push(move)
    //  受理した遷移を履歴に積む。apply(state, move)の後に呼ぶこと。
    //  「履歴がstateに適用済みの遷移列と一致している」ことがcompactionの前提になる
    void push(const Move& move) {
        log_.push_back(move);
        //  compactionはpushの後に行う。pushの前に行うと、
        //  まだ履歴に積まれていないmoveがstateに適用済みの状態で
        //  ベストを実体化してしまう。
        //  なお、この直後にmark()が呼ばれる場合もcompaction後の
        //  「空の履歴 + 現在状態」がベストとして正しく記録される
        if (log_.size() >= capacity_) compact_();
    }

    //  # mark()
    //  現在位置をベストとして記録する
    void mark() {
        best_length_ = log_.size();
        best_in_log_ = true;
    }

    //  # restore()
    //  ベスト状態への参照を返す。
    //  ベストが履歴内にある場合、stateをその位置まで巻き戻して返すため、
    //  呼び出し後のstateは現在状態ではなくベスト状態になる。
    //  冪等なので複数回呼んでよい
    const State& restore() {
        if (!best_in_log_) return best_;
        for (std::size_t i = log_.size(); i > best_length_; i--) {
            undo_(*state_, log_[i - 1]);
        }
        log_.resize(best_length_);
        return *state_;
    }

    //  # size()
    //  履歴に積まれているMoveの個数
    std::size_t size() const {
        return log_.size();
    }

    //  # compactions()
    //  compactionが起きた回数
    long long compactions() const {
        return compactions_;
    }

    //  # dump(os)
    //  履歴の使用量、確保しているメモリ、compactionの回数を出力する
    void dump(std::ostream& os = std::cerr) const {
        os << "# MoveLog\n";
        os << "  entries     : " << log_.size() << " / " << capacity_ << '\n';
        os << "  memory      : " << (capacity_ * sizeof(Move) / 1024 / 1024) << " MiB (sizeof(Move) = " << sizeof(Move)
           << ")\n";
        //  compactionが多いならcapacityを増やす
        os << "  compactions : " << compactions_ << '\n';
    }

  private:
    //  履歴が満杯になったとき、ベスト状態を実体化して履歴を捨てる
    void compact_() {
        if (best_in_log_) {
            best_ = *state_;
            for (std::size_t i = log_.size(); i > best_length_; i--) {
                undo_(best_, log_[i - 1]);
            }
            best_in_log_ = false;
        }
        log_.clear();
        best_length_ = 0;
        compactions_++;
    }

    State* state_;
    State best_;
    Undo undo_;
    std::vector<Move> log_;
    std::size_t capacity_;
    std::size_t best_length_ = 0;
    long long compactions_ = 0;
    //  ベストが「現在の履歴の途中」にあるか、「best_に実体化済み」か
    bool best_in_log_ = false;
};

//  # make_move_log<Move>(state, undo, capacity)
//  MoveはUndoの引数から推論できないため、明示的に指定する
template <class Move, class State, class Undo>
MoveLog<Move, State, Undo> make_move_log(State& state, Undo undo,
                                         std::size_t capacity = MoveLog<Move, State, Undo>::DEFAULT_CAPACITY) {
    return MoveLog<Move, State, Undo>(state, std::move(undo), capacity);
}

}  //  namespace nono
