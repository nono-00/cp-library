#include <cassert>
#include <iostream>

namespace nono {

namespace internal {

constexpr bool is_prime(unsigned long long n) {
    for (unsigned long long i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

}  //  namespace internal

template <unsigned long long MOD = 998244353>
class Modint {
  public:
    static_assert(internal::is_prime(MOD));

    constexpr Modint(unsigned long long value = 0): value_(value % MOD) {}

    constexpr Modint pow(long long exp) const {
        Modint result(1);
        Modint base(*this);
        while (exp > 0) {
            if (exp & 1) {
                result *= base;
            }
            base *= base;
            exp >>= 1;
        }
        return result;
    }

    constexpr Modint inv() const {
        assert(value_ != 0);
        return pow(MOD - 2);
    }

    void set(unsigned long long value) {
        if (value >= MOD) value %= MOD;
        value_ = value;
    }

    unsigned long long get() const {
        return value_;
    }

    friend constexpr bool operator==(const Modint lhs, const Modint rhs) {
        return lhs.value_ == rhs.value_;
    }

    constexpr Modint& operator+=(const Modint other) {
        this->value_ += other.value_;
        if (this->value_ >= MOD) this->value_ -= MOD;
        return *this;
    }

    constexpr Modint& operator-=(const Modint other) {
        this->value_ += MOD - other.value_;
        if (this->value_ >= MOD) this->value_ -= MOD;
        return *this;
    }

    constexpr Modint& operator*=(const Modint other) {
        this->value_ *= other.value_;
        if (this->value_ >= MOD) this->value_ %= MOD;
        return *this;
    }

    constexpr Modint& operator/=(const Modint other) {
        assert(other.value_ != 0);
        return *this *= other.inv();
    }

    constexpr friend Modint operator+(const Modint lhs, const Modint rhs) {
        return Modint(lhs) += rhs;
    }
    constexpr friend Modint operator-(const Modint lhs, const Modint rhs) {
        return Modint(lhs) -= rhs;
    }
    constexpr friend Modint operator*(const Modint lhs, const Modint rhs) {
        return Modint(lhs) *= rhs;
    }
    constexpr friend Modint operator/(const Modint lhs, const Modint rhs) {
        return Modint(lhs) /= rhs;
    }

    friend std::istream& operator>>(std::istream& stream, Modint& mint) {
        unsigned long long value;
        stream >> value;
        mint.set(value);
        return stream;
    }

    friend std::ostream& operator<<(std::ostream& stream, Modint mint) {
        stream << mint.get();
        return stream;
    }

  private:
    unsigned long long value_;
};

using Modint998244353 = Modint<998244353>;
using Modint1000000007 = Modint<1000000007>;

}  //  namespace nono
