#pragma once

#include <limits>

namespace nono {

template <class T = unsigned>
class Xorshift {
  public:
    using result_type = T;
    Xorshift(T value): value_(value) {}
    T operator()() {
        T value = value_;
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        return value_ = value;
    }
    static constexpr T min() {
        return std::numeric_limits<T>::min();
    }
    static constexpr T max() {
        return std::numeric_limits<T>::max();
    }

  private:
    T value_;
};

}  //  namespace nono
