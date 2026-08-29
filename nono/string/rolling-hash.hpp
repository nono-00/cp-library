#pragma once

#include <chrono>
#include <random>
#include <ranges>
#include <vector>

namespace nono {

///  # Rolling Hash
///  ロリハ.
///  based on <https://ei1333.github.io/library/string/rolling-hash.hpp>
template <uint64_t THRESHOLD = 10000000>
struct RollingHash {
    static constexpr unsigned long long mod = (1ull << 61ull) - 1;
    using uint128_t = __uint128_t;
    const unsigned long long base;
    std::vector<unsigned long long> power;
    explicit RollingHash(unsigned long long base = generate_base()): base(base), power{1} {}

    ///  # table(seq)
    ///  return hash table of seq
    ///  O(|seq|)
    template <std::ranges::random_access_range R>
    std::vector<unsigned long long> table(const R& seq) const {
        int n = seq.size();
        std::vector<unsigned long long> hashed(n + 1);
        for (int i = 0; i < n; i++) {
            hashed[i + 1] = add(mul(hashed[i], base), seq[i]);
        }
        return hashed;
    }
    ///  # hash(seq)
    ///  return hash of seq
    ///  O(|seq|)
    template <std::ranges::random_access_range R>
    unsigned long long hash(const R& seq) {
        int n = seq.size();
        unsigned long long val = 0;
        for (int i = 0; i < n; i++) {
            val = add(mul(val, base), seq[i]);
        }
        return val;
    }

    ///  # hash(seq)
    ///  return hash of seq (using table)
    ///  O(1)
    unsigned long long hash(const std::vector<unsigned long long>& s, int l, int r) {
        expand(r - l);
        return add(s[r], mod - mul(s[l], power[r - l]));
    }

    ///  # combine(h1, h2, h2len)
    ///  hash(s1) = h1
    ///  hash(s2) = h2
    ///  return hash of (s1 + s2)
    ///  O(1) or O(log |s2|)
    unsigned long long combine(unsigned long long h1, unsigned long long h2, unsigned long long h2len) {
        if (h2len <= THRESHOLD) {
            expand(h2len);
            return add(mul(h1, power[h2len]), h2);
        } else {
            return add(mul(h1, pow(base, h2len)), h2);
        }
    }

    ///  # lcp(a, l1, r1, b, l2, r2)
    ///  a := hash table of s
    ///  b := hash table of t
    ///  return lcp(s[l1:r1], t[l2:r2])
    ///  lcp := longest common prefix
    ///  O(log |seq|)
    int lcp(const std::vector<unsigned long long>& a, int l1, int r1, const std::vector<unsigned long long>& b, int l2,
            int r2) {
        int len = std::min(r1 - l1, r2 - l2);
        int low = 0, high = len + 1;
        while (high - low > 1) {
            int mid = (low + high) / 2;
            if (hash(a, l1, l1 + mid) == hash(b, l2, l2 + mid))
                low = mid;
            else
                high = mid;
        }
        return low;
    }

    ///  # expand(n)
    inline void expand(int n) {
        if ((int)power.size() < n + 1) {
            int pre_n = (int)power.size();
            power.resize(n + 1);
            for (int i = pre_n - 1; i < n; i++) {
                power[i + 1] = mul(power[i], base);
            }
        }
    }

  private:
    static inline unsigned long long add(unsigned long long a, unsigned long long b) {
        if ((a += b) >= mod) a -= mod;
        return a;
    }
    static inline unsigned long long mul(unsigned long long a, unsigned long long b) {
        uint128_t c = (uint128_t)a * b;
        return add(c >> 61, c & mod);
    }
    static inline unsigned long long pow(unsigned long long base, unsigned long long exp) {
        unsigned long long result = 1;
        while (exp) {
            if (exp & 1) result = mul(result, base);
            base = mul(base, base);
            exp >>= 1;
        }
        return result;
    }
    static inline unsigned long long generate_base() {
        std::mt19937_64 mt(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<unsigned long long> rand(1, RollingHash::mod - 1);
        return rand(mt);
    }
};

}  //  namespace nono
