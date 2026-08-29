#pragma once

#include <algorithm>

#include "primes.hpp"

namespace nono {

//  https://www.acmicpc.net/problem/17633
//  最小で幾つの平方数の和で表されるか？
int count_square_sum_terms(unsigned long long n) {
    auto factors = prime_factors(n);
    std::sort(std::begin(factors), std::end(factors));
    {
        bool ok = true;
        for (int l = 0, r = 0; l < std::ssize(factors); l = r) {
            while (r < std::ssize(factors) && factors[l] == factors[r]) r++;
            ok &= (r - l) % 2 == 0;
        }
        if (ok) return 1;
    }
    {
        //  2か判定
        //  https://manabitimes.jp/math/844
        //  4k+3型の素数の指数が全て偶数か？
        bool ok = true;
        for (int l = 0, r = 0; l < std::ssize(factors); l = r) {
            while (r < std::ssize(factors) && factors[l] == factors[r]) r++;
            if (factors[l] % 4 == 3) {
                ok &= (r - l) % 2 == 0;
            }
        }
        if (ok) return 2;
    }
    {
        //  3か判定
        //  https://ja.wikipedia.org/wiki/%E4%B8%89%E5%80%8B%E3%81%AE%E5%B9%B3%E6%96%B9%E6%95%B0%E3%81%AE%E5%92%8C
        //  4で割り切ったあと、8で割ったあまりが7でなければ良い
        unsigned long long x = n;
        while (x % 4 == 0) x /= 4;
        if (x % 8 != 7) return 3;
    }
    //  必ず4つで表せる
    return 4;
}

}  //  namespace nono
