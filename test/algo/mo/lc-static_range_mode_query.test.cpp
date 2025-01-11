#define PROBLEM "https://judge.yosupo.jp/problem/static_range_mode_query"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "../../../nono/algo/mo.hpp"

namespace nono {

struct ManageMaxMode {
    int n;
    int SQRT;
    std::vector<int> freq;
    std::vector<std::vector<int>> freq_freq;
    std::vector<int> max_freq;
    ManageMaxMode(int n, int max)
        : n(n),
          SQRT(std::max((int)std::sqrt(n), 1)),
          freq(n),
          freq_freq((n + SQRT - 1) / SQRT, std::vector<int>(max + 1)),
          max_freq((n + SQRT - 1) / SQRT) {
        for (int i = 0; i < n; i++) freq_freq[i / SQRT][0]++;
    }
    void add(int v) {
        assert(0 <= v && v < n);
        int i = v / SQRT;
        freq_freq[i][freq[v]]--;
        freq_freq[i][++freq[v]]++;
        if (freq_freq[i][max_freq[i] + 1]) max_freq[i]++;
    }
    void erase(int v) {
        assert(0 <= v && v < n);
        int i = v / SQRT;
        freq_freq[i][freq[v]]--;
        freq_freq[i][--freq[v]]++;
        assert(freq[v] >= 0);
        if (freq_freq[i][max_freq[i]] == 0) max_freq[i]--;
    }
    std::pair<int, int> solve() {
        int i = (std::max_element(max_freq.begin(), max_freq.end()) - max_freq.begin());
        for (int j = SQRT * i;; j++) {
            if (max_freq[i] == freq[j]) return {j, freq[j]};
        }
    }
};

void solve() {
    constexpr int N = 100000;
    constexpr int Q = 100000;
    int n, q;
    std::cin >> n >> q;
    std::vector<int> a(n);
    for (auto& v: a) std::cin >> v;
    std::vector<std::pair<int, int>> querys(q);
    for (auto& [l, r]: querys) std::cin >> l >> r;
    std::vector<int> b(n);
    auto c = a;
    std::sort(c.begin(), c.end());
    int max_cnt = 0;
    for (int l = 0, r = 0; l < n; l = r) {
        while (r < n && c[l] == c[r]) r++;
        max_cnt = std::max(max_cnt, r - l);
    }
    c.erase(std::unique(c.begin(), c.end()), c.end());
    for (int i = 0; i < n; i++) {
        b[i] = std::lower_bound(c.begin(), c.end(), a[i]) - c.begin();
    }
    ManageMaxMode data(c.size(), max_cnt + 10);
    auto add = [&](int i, bool) {
        assert(0 <= i && i < n);
        data.add(b[i]);
    };
    auto erase = [&](int i, bool) {
        assert(0 <= i && i < n);
        data.erase(b[i]);
    };
    std::vector<std::pair<int, int>> ans(q);
    auto solve = [&](int i) {
        assert(0 <= i && i < q);
        ans[i] = data.solve();
    };
    mo<N, Q>(querys, add, erase, solve);
    for (auto [mode, freq]: ans) {
        std::cout << c[mode] << ' ' << freq << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
