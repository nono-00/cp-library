#pragma once

#include <utility>

namespace nono {

std::pair<long long, long long> safe_div_mod(long long x, long long m) {
    long long p = x / m;
    long long q = x % m;
    if (q < 0) p--, q += m;
    return {p, q};
}

long long safe_div(long long x, long long m) {
    return safe_div_mod(x, m).first;
}

long long safe_mod(long long x, long long m) {
    return safe_div_mod(x, m).second;
}

}  //  namespace nono
