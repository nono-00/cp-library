#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/2446"
#define ERROR "1e-7"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

#include "../../../nono/math/zeta-mobius-transform.hpp"

namespace nono {

void solve() {
    int n;
    long long m;
    std::cin >> n >> m;
    std::vector<long long> a(n);
    std::vector<int> p(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    for (int i = 0; i < n; i++) std::cin >> p[i];

    std::vector<long long> dp(1 << n);
    for (int state = 1; state < (1 << n); state++) {
        long long lcm = 1;
        for (int i = 0; i < n; i++) {
            if (state & (1 << i)) {
                long long g = std::gcd(lcm, a[i]);
                if ((long double)lcm * (long double)a[i] / (long double)g <= 2e18) {
                    lcm = (lcm / g) * a[i];
                } else {
                    lcm = m + 1;
                }
            }
        }
        dp[state] = m / lcm;
    }

    subset_mobius_transform(dp);

    long double ans = 0;
    for (int state = 0; state < (1 << n); state++) {
        long double prob = 1;
        for (int i = 0; i < n; i++) {
            if (state & (1 << i)) {
                prob *= (long double)p[i] / 100;
            } else {
                prob *= (long double)(100 - p[i]) / 100;
            }
        }
        ans += prob * std::abs(dp[state]);
    }
    std::cout << std::fixed << std::setprecision(16) << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
