#pragma once

#include "nono/math/modint.hpp"

#include <cassert>
#include <vector>

namespace nono {

/// THRESHOLD: 10^7
/// if THRESHOLD <= n, calculated by naive
template <int m, int THRESHOLD = 10000000>
class Binomial {
    using Mint = StaticModint<m>;

  public:
    Binomial(): size_(0), fact_(1, 1), fact_inv_(1, 1) {}

    ///  # binom(n, k)
    ///  nCk
    Mint binom(int n, int k) {
        assert(0 <= k);
        assert(k <= n);
        if (THRESHOLD <= n) return binom_naive(n, k);
        extend(n);
        return fact_[n] * fact_inv_[n - k] * fact_inv_[k];
    }

    ///  # fact(n)
    ///  n!
    Mint fact(int n) {
        assert(0 <= n);
        extend(n);
        return fact_[n];
    }

    ///  # fact_inv(n)
    ///  inv(n!)
    Mint fact_inv(int n) {
        assert(0 <= n);
        extend(n);
        return fact_inv_[n];
    }

    ///  # perm(n, k)
    ///  nPk
    Mint perm(int n, int k) {
        assert(0 <= k);
        assert(k <= n);
        if (THRESHOLD <= n) return perm_naive(n, k);
        extend(n);
        return fact_[n] * fact_inv_[n - k];
    }

  private:
    Mint binom_naive(int n, int k) {
        Mint res = 1;
        for (int i = 0; i < k; i++) {
            res *= (n - i);
            res /= (i + 1);
        }
        return res;
    }
    Mint perm_naive(int n, int k) {
        Mint res = 1;
        for (int i = 0; i < k; i++) {
            res *= (n - i);
        }
        return res;
    }

    void extend(int n) {
        if (size_ < n) {
            fact_.resize(n + 1);
            for (int i = size_ + 1; i < n + 1; i++) {
                fact_[i] = i * fact_[i - 1];
            }
            fact_inv_.resize(n + 1);
            fact_inv_[n] = fact_[n].inv();
            for (int i = n - 1; i > size_; i--) {
                fact_inv_[i] = (i + 1) * fact_inv_[i + 1];
            }
            size_ = n;
        }
    }
    int size_;
    std::vector<Mint> fact_;
    std::vector<Mint> fact_inv_;
};

using Binomial998244353 = Binomial<998244353>;
using Binomial1000000007 = Binomial<1000000007>;

}  //  namespace nono
