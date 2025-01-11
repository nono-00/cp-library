#define IGNORE
#include <iostream>
#include <string>
#include <vector>

#include "../../../nono/math/modint.hpp"
#include "../../../nono/string/eertree.hpp"

namespace nono {

constexpr int N = 1000005;
using Mint = Modint998244353;
decltype(Eertree{}.nodes()) nodes;

Mint G(int i);

Mint F(int i) {
    static bool already[N]{};
    static Mint memo[N]{};
    if (already[i]) return memo[i];
    already[i] = true;
    if (nodes[i].len == 1) return memo[i] = 1;
    return memo[i] = G(nodes[i].parent) + F(nodes[i].link);
}

Mint G(int i) {
    static bool already[N]{};
    static Mint memo[N]{};
    if (already[i]) return memo[i];
    already[i] = true;
    if (nodes[i].len == 0) return memo[i] = 1;
    if (nodes[i].len == 1) return memo[i] = 2;
    return memo[i] = 2 * F(i);
}

void solve() {
    std::string s;
    std::cin >> s;
    Eertree tree;
    tree.add(std::vector(s.begin(), s.end()));
    nodes = tree.nodes();
    std::vector<bool> already1(nodes.size());
    std::vector<bool> already2(nodes.size());
    std::vector<Mint> memo1(nodes.size());
    std::vector<Mint> memo2(nodes.size());
    Mint ans = 0;
    for (auto id: tree.node_id()) {
        ans += F(id);
    }
    std::cout << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
