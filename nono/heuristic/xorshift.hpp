#pragma once

#include <limits>

namespace nono {

//  # Xorshift<T>
//  xorshiftによる擬似乱数生成器。std::uniform_random_bit_generatorを満たすので、
//  <random>の分布やAnnealerのRngに渡せる
//  シフト量は32bit版のxorshiftと同じ (13, 17, 5)
//  Tには符号なし整数型を指定する(符号付きだとstd::uniform_random_bit_generatorを満たさない)
//  コンストラクタに0を渡すと、0しか返さなくなる
template <class T = unsigned>
class Xorshift {
  public:
    using result_type = T;
    Xorshift(T value): value_(value) {}
    //  # operator()()
    //  次の乱数を返す
    T operator()() {
        T value = value_;
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        return value_ = value;
    }
    //  # min()
    //  Tの最小値
    static constexpr T min() {
        return std::numeric_limits<T>::min();
    }
    //  # max()
    //  Tの最大値
    static constexpr T max() {
        return std::numeric_limits<T>::max();
    }

  private:
    T value_;
};

}  //  namespace nono
