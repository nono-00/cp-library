#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/typical90/tasks/typical90_bx"
#include <iostream>
#include <numeric>
#include <set>

#include "utility/ring-array.hpp"

int main() {
    using namespace nono;
    int n;
    std::cin >> n;
    RingArray<long long> a(n);
    for (auto& v: a) {
        std::cin >> v;
    }

    long long circle = std::accumulate(a.cbegin(), a.cend(), 0LL);
    if (circle % 10 != 0) {
        std::cout << "No" << std::endl;
        return 0;
    }
    std::set<long long> s;
    s.insert(0);
    {
        long long sum = 0;
        for (int i = 0; i < 2 * n; i++) {
            sum += a[i];
            s.insert(sum);
        }
    }
    {
        long long sum = 0;
        for (int i = 0; i < n; i++) {
            if (s.count(sum + circle / 10)) {
                std::cout << "Yes" << std::endl;
                return 0;
            }
            sum += a[i];
        }
    }
    std::cout << "No" << std::endl;
}
