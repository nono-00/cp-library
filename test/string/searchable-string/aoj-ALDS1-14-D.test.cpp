#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/lesson/1/ALDS1/all/ALDS1_14_D"
#include <iostream>

#include "nono/string/searchable-string.hpp"

namespace nono {

void solve() {
    SearchableString s;
    {
        std::string t;
        std::cin >> t;
        s = SearchableString(std::move(t));
    }

    int q;
    std::cin >> q;
    while (q--) {
        std::string t;
        std::cin >> t;
        std::cout << s.contains(t) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
