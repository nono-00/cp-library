#define PROBLEM "https://judge.yosupo.jp/problem/longest_increasing_subsequence"
#include <iostream>
#include <vector>

#include "../../../nono/algo/longest-increasing-subseq.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    auto result = longest_increasing_subseq(a);
    std::cout << result.size() << '\n';
    for (int i = 0; i < std::ssize(result); i++) {
        std::cout << result[i] << " \n"[i + 1 == std::ssize(result)];
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
