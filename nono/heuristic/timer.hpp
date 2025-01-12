#pragma once

#include <chrono>

namespace nono {

///  # Timer
class Timer {
  public:
    ///  # Timer(limit)
    ///  1秒にしたいなら1000.
    Timer(long long limit): limit_(limit) {
        using namespace std::chrono;
        start_ = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }

    ///  # timelimit()
    bool timelimit() const {
        return remain() < 0;
    }

    ///  # now()
    ///  コンストラクタ呼び出しからどれだけ時間が経ったか
    long long now() const {
        return limit_ - remain();
    }

    ///  # remain()
    ///  timelimit()までどれだけか
    long long remain() const {
        using namespace std::chrono;
        return limit_ - (duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - start_);
    }

  private:
    long long limit_;
    long long start_;
};

}  //  namespace nono
