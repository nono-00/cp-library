#define PROBLEM "https://judge.yosupo.jp/problem/unionfind_with_potential_non_commutative_group"
#include <array>
#include <iostream>
#include <utility>

#include "nono/ds/potentialized-union-find.hpp"
#include "nono/math/modint.hpp"

namespace nono {

using Mint = Modint998244353;

struct Mat {
    using Value = std::array<std::array<Mint, 2>, 2>;
    static Value op(Value lhs, Value rhs) {
        Value result{};
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    result[i][k] += lhs[i][j] * rhs[j][k];
                }
            }
        }
        return result;
    }
    static Value e() {
        Value result{};
        result[0][0] = result[1][1] = 1;
        return result;
    }
    static Value inv(Value value) {
        Value result;
        result[0][0] = value[1][1];
        result[1][1] = value[0][0];
        result[0][1] = -1 * value[0][1];
        result[1][0] = -1 * value[1][0];
        return result;
    }
};

struct Group {
    using Value = std::pair<Mat::Value, Mat::Value>;
    static Value op(Value lhs, Value rhs) {
        Value result{};
        result.first = Mat::op(lhs.first, rhs.first);
        result.second = Mat::op(rhs.second, lhs.second);
        return result;
    }
    static Value e() {
        return std::pair{Mat::e(), Mat::e()};
    }
    static Value inv(Value value) {
        return {Mat::inv(value.first), Mat::inv(value.second)};
    }
};

void solve() {
    int n, q;
    std::cin >> n >> q;
    PotentializedUnionFind<Mat> puf(n);

    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int x, y;
            Mat::Value mat;
            std::cin >> x >> y;
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    std::cin >> mat[i][j];
                }
            }
            if (puf.merge(x, y, mat)) {
                std::cout << 1 << '\n';
            } else if (puf.potential(x, y) == mat) {
                std::cout << 1 << '\n';
            } else {
                std::cout << 0 << '\n';
            }
        } else {
            int x, y;
            std::cin >> x >> y;
            auto result = puf.potential(x, y);
            if (!result) {
                std::cout << -1 << std::endl;
            } else {
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        std::cout << (*result)[i][j] << " \n"[i == 1 && j == 1];
                    }
                }
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
