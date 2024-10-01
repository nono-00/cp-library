#define PROBLEM "https://judge.yosupo.jp/problem/number_of_substrings"
#include <iostream>
#include <string>

#include "nono/string/lcp-array.hpp"
#include "nono/string/suffix-array.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    int n = s.size();
    auto sa = suffix_array(s);
    auto lcp = lcp_array(s, sa);
    long long ans = n - sa[0];
    for (int i = 1; i < n; i++) {
        ans += (int)s.size() - sa[i] - lcp[i - 1];
    }
    std::cout << ans << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
