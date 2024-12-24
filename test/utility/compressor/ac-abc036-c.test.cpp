#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc036/tasks/abc036_c"
#include <iostream>
#include <vector>

#include "../../../nono/utility/compressor.hpp"

namespace nono {

void solve() {
    using namespace nono;
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    Compressor compresser(a);
    for (int v: a) {
        std::cout << compresser.compress(v) << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
