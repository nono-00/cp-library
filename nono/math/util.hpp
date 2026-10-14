#pragma once

#include <utility>

namespace nono {

//  # safe_div_mod(x, m)
//  x = p * m + q, 0 <= q < m を満たす (p, q) を返す。xが負でも、商は切り捨て、余りは非負になる
//  m > 0 でないと壊れる
std::pair<long long, long long> safe_div_mod(long long x, long long m) {
    long long p = x / m;
    long long q = x % m;
    if (q < 0) p--, q += m;
    return {p, q};
}

//  # safe_div(x, m)
//  floor(x / m) を返す
//  m > 0 でないと壊れる
long long safe_div(long long x, long long m) {
    return safe_div_mod(x, m).first;
}

//  # safe_mod(x, m)
//  x mod m を [0, m) で返す
//  m > 0 でないと壊れる
long long safe_mod(long long x, long long m) {
    return safe_div_mod(x, m).second;
}

}  //  namespace nono
