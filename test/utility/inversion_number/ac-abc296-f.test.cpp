#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc296/tasks/abc296_f"
#include <iostream>
#include <set>
#include <vector>

#include "nono/utility/inversion_number.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<int> a(n), b(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    for (int i = 0; i < n; i++) std::cin >> b[i];
    std::vector<int> count(n + 1);
    for (int i = 0; i < n; i++) {
        count[a[i]]++;
    }
    for (int i = 0; i < n; i++) {
        count[b[i]]--;
    }
    for (int i = 1; i <= n; i++) {
        if (count[i] != 0) {
            std::cout << "No" << std::endl;
            return;
        }
    }
    std::set<int> s(a.begin(), a.end());
    if ((int)s.size() != n) {
        std::cout << "Yes" << std::endl;
        return;
    }
    if ((inversion_number(a) + inversion_number(b)) % 2 == 0) {
        std::cout << "Yes" << std::endl;
    } else {
        std::cout << "No" << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
