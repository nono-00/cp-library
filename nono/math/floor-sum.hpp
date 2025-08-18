#pragma once

#include <utility>

#include "util.hpp"

namespace nono {

long long floor_sum(long long n, long long m, long long a, long long b) {
    if (a < 0 || m <= a) {
        auto [a1, a2] = safe_div_mod(a, m);
        return safe_div(n * (n - 1), 2) * a1 + floor_sum(n, m, a2, b);
    }
    if (b < 0 || m <= b) {
        auto [b1, b2] = safe_div_mod(b, m);
        return n * b1 + floor_sum(n, m, a, b2);
    }
    if (a == 0) {
        return n * safe_div(b, m);
    }
    long long k = safe_div(a * (n - 1) + b, m);
    return k * n - floor_sum(k, a, m, m + a - b - 1);
}

}  //  namespace nono
