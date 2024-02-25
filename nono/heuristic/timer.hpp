#pragma once

#include <chrono>

namespace nono {

//  brief:
//  - Timer
//
//  note:
//  - milliseconds
//  - 1秒を測りたい場合は1000を引数に与える
class Timer {
  public:
    //  brief:
    //  - コンストラクタ
    //
    //  param:
    //  - `limit`: 制限時間. 1秒にしたいなら1000.
    Timer(long long limit): limit_(limit) {
        using namespace std::chrono;
        start_ = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }

    //  brief:
    //  - 制限時間を超えたかどうか
    bool timelimit() const {
        return remain() < 0;
    }

    //  brief:
    //  - コンストラクタ呼び出しからどれだけ時間が経ったか
    long long now() const {
        return limit_ - remain();
    }

    //  brief:
    //  - 制限時間までのこりどれだけか
    long long remain() const {
        using namespace std::chrono;
        return limit_ - (duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - start_);
    }

  private:
    long long limit_;
    long long start_;
};

}  //  namespace nono
