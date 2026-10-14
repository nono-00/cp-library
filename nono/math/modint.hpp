#pragma once

#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>

namespace nono {

namespace modint_internal {

//  x mod m を [0, m) で返す
//  1 <= m でないと壊れる
constexpr long long safe_mod(long long x, long long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

//  Fast modular multiplication by barrett reduction
//  Reference: https://en.wikipedia.org/wiki/Barrett_reduction
//  NOTE: reconsider after Ice Lake
struct barrett {
    unsigned int _m;
    unsigned long long im;

    //  1 <= m でないと壊れる
    explicit barrett(unsigned int m): _m(m), im((unsigned long long)(-1) / m + 1) {}

    //  mを返す
    unsigned int umod() const {
        return _m;
    }

    //  a * b % m を返す
    //  0 <= a < m, 0 <= b < m でないと壊れる
    unsigned int mul(unsigned int a, unsigned int b) const {
        //  [1] m = 1
        //  a = b = im = 0, so okay

        //  [2] m >= 2
        //  im = ceil(2^64 / m)
        //  -> im * m = 2^64 + r (0 <= r < m)
        //  let z = a*b = c*m + d (0 <= c, d < m)
        //  a*b * im = (c*m + d) * im = c*(im*m) + d*im = c*2^64 + c*r + d*im
        //  c*r + d*im < m * m + m * im < m * m + 2^64 + m <= 2^64 + m * (m + 1) < 2^64 * 2
        //  ((ab * im) >> 64) == c or c + 1
        unsigned long long z = a;
        z *= b;
        unsigned long long x = (unsigned long long)(((unsigned __int128)(z)*im) >> 64);
        unsigned long long y = x * _m;
        return (unsigned int)(z - y + (z < y ? _m : 0));
    }
};

//  (x ** n) % m を返す
//  0 <= n, 1 <= m でないと壊れる
constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    if (m == 1) return 0;
    unsigned int _m = (unsigned int)(m);
    unsigned long long r = 1;
    unsigned long long y = safe_mod(x, m);
    while (n) {
        if (n & 1) r = (r * y) % _m;
        y = (y * y) % _m;
        n >>= 1;
    }
    return r;
}

//  Reference:
//  M. Forisek and J. Jancina,
//  Fast Primality Testing for Integers That Fit into a Machine Word
//  0 <= n でないと壊れる
constexpr bool is_prime_constexpr(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 7 || n == 61) return true;
    if (n % 2 == 0) return false;
    long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    constexpr long long bases[3] = {2, 7, 61};
    for (long long a: bases) {
        long long t = d;
        long long y = pow_mod_constexpr(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = y * y % n;
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
template <int n>
constexpr bool is_prime = is_prime_constexpr(n);

//  g = gcd(a, b), xa = g (mod b), 0 <= x < b/g を満たす pair(g, x) を返す
//  1 <= b でないと壊れる
constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    //  Contracts:
    //  [1] s - m0 * a = 0 (mod b)
    //  [2] t - m1 * a = 0 (mod b)
    //  [3] s * |m1| + t * |m0| <= b
    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;  //  |m1 * u| <= |m1| * s <= b

        //  [3]:
        //  (s - t * u) * |m1| + t * |m0 - m1 * u|
        //  <= s * |m1| - t * u * |m1| + t * (|m0| + |m1| * u)
        //  = s * |m1| + t * |m0| <= b

        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    //  by [3]: |m0| <= b/g
    //  by g != b: |m0| < b/g
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value || std::is_same<T, __int128>::value, std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value || std::is_same<T, unsigned __int128>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value, __uint128_t, unsigned __int128>;

template <class T>
using is_integral =
    typename std::conditional<std::is_integral<T>::value || is_signed_int128<T>::value || is_unsigned_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_signed_int =
    typename std::conditional<(is_integral<T>::value && std::is_signed<T>::value) || is_signed_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value && std::is_unsigned<T>::value) || is_unsigned_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value, make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value, std::make_unsigned<T>, std::common_type<T>>::type>::type;

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T>
using to_unsigned_t = typename to_unsigned<T>::type;

}  //  namespace modint_internal

}  //  namespace nono

namespace nono {

//  # StaticModint<m>
//  mod m の整数。aclベースのmodint. cin, coutによる入出力に対応.
//  四則演算、++, --、単項の+, -、==, != が使える。除算は逆元を掛けるので、inv()と同じ条件で壊れる
//  mはコンパイル時定数で、1 <= m
template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct StaticModint {
    using Mint = StaticModint;

  public:
    //  # mod()
    //  mを返す
    static constexpr int mod() {
        return m;
    }

    //  # raw(v)
    //  vを、modを取らずにそのまま値にする
    //  0 <= v < m でないと壊れる
    static Mint raw(int v) {
        Mint x;
        x._v = v;
        return x;
    }

    StaticModint(): _v(0) {}
    //  # StaticModint(v)
    //  vを mod m で [0, m) に直して持つ。負の値でもよい
    template <class T, modint_internal::is_signed_int_t<T>* = nullptr>
    StaticModint(T v) {
        long long x = (long long)(v % (long long)(umod()));
        if (x < 0) x += umod();
        _v = (unsigned int)(x);
    }
    template <class T, modint_internal::is_unsigned_int_t<T>* = nullptr>
    StaticModint(T v) {
        _v = (unsigned int)(v % umod());
    }

    //  # val()
    //  値を [0, m) で返す
    unsigned int val() const {
        return _v;
    }

    Mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    Mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    Mint operator++(int) {
        Mint result = *this;
        ++*this;
        return result;
    }
    Mint operator--(int) {
        Mint result = *this;
        --*this;
        return result;
    }

    Mint& operator+=(const Mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    Mint& operator-=(const Mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    Mint& operator*=(const Mint& rhs) {
        unsigned long long z = _v;
        z *= rhs._v;
        _v = (unsigned int)(z % umod());
        return *this;
    }
    Mint& operator/=(const Mint& rhs) {
        return *this = *this * rhs.inv();
    }

    Mint operator+() const {
        return *this;
    }
    Mint operator-() const {
        return Mint() - *this;
    }

    //  # pow(n)
    //  n乗
    //  0 <= n でないと壊れる
    //  O(log n)
    Mint pow(long long n) const {
        assert(0 <= n);
        Mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    //  # inv()
    //  逆元
    //  mが素数のときは、値が0だと壊れる。素数でないときは、値とmが互いに素でないと壊れる
    //  O(log m)
    Mint inv() const {
        if (prime) {
            assert(_v);
            return pow(umod() - 2);
        } else {
            auto eg = modint_internal::inv_gcd(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }

    friend Mint operator+(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) += rhs;
    }
    friend Mint operator-(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) -= rhs;
    }
    friend Mint operator*(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) *= rhs;
    }
    friend Mint operator/(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) /= rhs;
    }
    friend bool operator==(const Mint& lhs, const Mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const Mint& lhs, const Mint& rhs) {
        return lhs._v != rhs._v;
    }
    friend std::istream& operator>>(std::istream& is, Mint& v) {
        long long x;
        is >> x;
        v = Mint(x);
        return is;
    }
    friend std::ostream& operator<<(std::ostream& os, const Mint& v) {
        return os << v.val();
    }

  private:
    unsigned int _v;
    static constexpr unsigned int umod() {
        return m;
    }
    static constexpr bool prime = modint_internal::is_prime<m>;
};

//  # DynamicModint<id>
//  modを実行時に決めるmodint。idごとに別のmodを持てる。使い方はStaticModintと同じ
//  modの初期値は998244353
template <int id>
struct DynamicModint {
    using Mint = DynamicModint;

  public:
    //  # mod()
    //  現在のmodを返す
    static int mod() {
        return (int)(bt.umod());
    }

    //  # set_mod(m)
    //  modをmにする。すでに作った値は、新しいmodで取り直されない
    //  1 <= m でないと壊れる
    static void set_mod(int m) {
        assert(1 <= m);
        bt = modint_internal::barrett(m);
    }

    //  # raw(v)
    //  vを、modを取らずにそのまま値にする
    //  0 <= v < mod() でないと壊れる
    static Mint raw(int v) {
        Mint x;
        x._v = v;
        return x;
    }

    DynamicModint(): _v(0) {}
    //  # DynamicModint(v)
    //  vを現在のmodで [0, mod()) に直して持つ。負の値でもよい
    template <class T, modint_internal::is_signed_int_t<T>* = nullptr>
    DynamicModint(T v) {
        long long x = (long long)(v % (long long)(mod()));
        if (x < 0) x += mod();
        _v = (unsigned int)(x);
    }
    template <class T, modint_internal::is_unsigned_int_t<T>* = nullptr>
    DynamicModint(T v) {
        _v = (unsigned int)(v % mod());
    }

    //  # val()
    //  値を [0, mod()) で返す
    unsigned int val() const {
        return _v;
    }

    Mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    Mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    Mint operator++(int) {
        Mint result = *this;
        ++*this;
        return result;
    }
    Mint operator--(int) {
        Mint result = *this;
        --*this;
        return result;
    }

    Mint& operator+=(const Mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    Mint& operator-=(const Mint& rhs) {
        _v += mod() - rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    Mint& operator*=(const Mint& rhs) {
        _v = bt.mul(_v, rhs._v);
        return *this;
    }
    Mint& operator/=(const Mint& rhs) {
        return *this = *this * rhs.inv();
    }

    Mint operator+() const {
        return *this;
    }
    Mint operator-() const {
        return Mint() - *this;
    }

    //  # pow(n)
    //  n乗
    //  0 <= n でないと壊れる
    //  O(log n)
    Mint pow(long long n) const {
        assert(0 <= n);
        Mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    //  # inv()
    //  逆元
    //  値とmodが互いに素でないと壊れる
    //  O(log mod())
    Mint inv() const {
        auto eg = modint_internal::inv_gcd(_v, mod());
        assert(eg.first == 1);
        return eg.second;
    }

    friend Mint operator+(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) += rhs;
    }
    friend Mint operator-(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) -= rhs;
    }
    friend Mint operator*(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) *= rhs;
    }
    friend Mint operator/(const Mint& lhs, const Mint& rhs) {
        return Mint(lhs) /= rhs;
    }
    friend bool operator==(const Mint& lhs, const Mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const Mint& lhs, const Mint& rhs) {
        return lhs._v != rhs._v;
    }
    friend std::istream& operator>>(std::istream& is, Mint& v) {
        long long x;
        is >> x;
        v = Mint(x);
        return is;
    }
    friend std::ostream& operator<<(std::ostream& os, const Mint& v) {
        return os << v.val();
    }

  private:
    unsigned int _v;
    static modint_internal::barrett bt;
    static unsigned int umod() {
        return bt.umod();
    }
};
template <int id>
modint_internal::barrett DynamicModint<id>::bt(998244353);

//  Modint998244353, Modint1000000007, Modint
//  よく使うmodの別名。Modintは実行時にmodを決める版
using Modint998244353 = StaticModint<998244353>;
using Modint1000000007 = StaticModint<1000000007>;
using Modint = DynamicModint<-1>;

}  //  namespace nono
