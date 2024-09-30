#define PROBLEM "https://judge.yosupo.jp/problem/lyndon_factorization"
#include <iostream>
#include <random>
#include <string>

#include "nono/string/lyndon-factorize.hpp"

namespace nono {

bool is_lyndon(std::string s) {
    int n = s.size();
    for (int i = 1; i < n; i++) {
        auto t = s.substr(i, n);
        if (s >= t) return false;
    }
    return true;
}

std::vector<int> naive(std::string s) {
    int n = s.size();
    std::vector<int> result;
    for (int l = 0; l < n;) {
        int max_r = -1;
        result.push_back(l);
        for (int r = l + 1; r <= n; r++) {
            if (is_lyndon(s.substr(l, r - l))) max_r = r;
        }
        l = max_r;
    }
    result.push_back(n);
    return result;
}

void test() {
    std::mt19937 mt(std::random_device{}());
    while (true) {
        std::string s;
        int n = mt() % 20 + 1;
        for (int i = 0; i < n; i++) s.push_back('a' + (mt() % 5));
        std::cerr << " ============== " << std::endl;
        std::cerr << s << std::endl;
        auto result1 = lyndon_factorize(s);
        auto result2 = naive(s);
        for (int i = 0; i < std::ssize(result1); i++) {
            std::cerr << result1[i] << " \n"[i + 1 == std::ssize(result1)];
        }
        for (int i = 0; i < std::ssize(result2); i++) {
            std::cerr << result2[i] << " \n"[i + 1 == std::ssize(result2)];
        }
        if (result1 != result2) break;
    }
}

void solve() {
    std::string s;
    std::cin >> s;
    auto result1 = lyndon_factorize(s);
    for (int i = 0; i < std::ssize(result1); i++) {
        std::cout << result1[i] << " \n"[i + 1 == std::ssize(result1)];
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
