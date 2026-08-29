#pragma once

#include <chrono>

namespace nono {

///  # Timer
///  時計はsteady_clockを使う。high_resolution_clockは単調増加が保証されておらず、
///  実装によってはsystem_clockの別名になるため、計測中の時刻補正で経過時間が狂いうる
class Timer {
  public:
    ///  # Timer(duration_milliseconds)
    ///  1秒にしたいなら1000
    explicit Timer(double duration_milliseconds)
        : duration_milliseconds_(duration_milliseconds),
          start_(std::chrono::steady_clock::now()) {}

    ///  # expired()
    ///  制限時間を超えたか
    bool expired() const {
        return progress() > 1.;
    }

    ///  # progress()
    ///  経過時間 / 制限時間。0から始まり、1を超えうる
    double progress() const {
        return elapsed() / duration_milliseconds_;
    }

    ///  # elapsed()
    ///  経過ミリ秒。ミリ秒未満も表現される
    double elapsed() const {
        return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_).count();
    }

  private:
    double duration_milliseconds_;
    std::chrono::steady_clock::time_point start_;
};

}  //  namespace nono
