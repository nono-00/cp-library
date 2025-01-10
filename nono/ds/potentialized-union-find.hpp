#pragma once

#include <algorithm>
#include <cassert>
#include <optional>
#include <vector>

namespace nono {

///  # Potentialized Union Find
///  UnionFind with Potential.
///  可換/非可換どちらでも動く.
template <class G>
class PotentializedUnionFind {
    using T = G::Value;
    using Result = std::optional<T>;

  public:
    PotentializedUnionFind() = default;
    PotentializedUnionFind(int n): n_(n), data_(n, -1), potential_(n, G::e()) {}

    ///  # leader(x)
    ///  return leader(group of x)
    ///  O(logn)
    int leader(int x) {
        assert(0 <= x && x < n_);
        if (data_[x] < 0) {
            return x;
        }
        potential_[x] = G::op(potential_[x], potential(data_[x]));
        return data_[x] = leader(data_[x]);
    }

    ///  # merge(lhs, rhs, diff)
    ///  頂点 `lhs` を基準とした頂点 `rhs` のポテンシャルが `diff`
    ///  となるように辺を追加する
    ///  O(logn)
    bool merge(int lhs, int rhs, T diff) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        if (same(lhs, rhs)) {
            return false;
        }
        diff = G::op(G::inv(potential(rhs)), G::op(diff, potential(lhs)));
        lhs = leader(lhs);
        rhs = leader(rhs);
        //  size(lhs) > size(rhs)とする
        if (-data_[lhs] < -data_[lhs]) {
            std::swap(lhs, rhs);
            diff = G::inv(diff);
        }
        data_[lhs] += data_[rhs];
        data_[rhs] = lhs;
        potential_[rhs] = diff;

        return true;
    }

    ///  # potential(x)
    ///  leader(x) を基準とした x のポテンシャルを取得する
    ///  O(logn)
    T potential(int x) {
        assert(0 <= x && x < n_);
        leader(x);
        return potential_[x];
    }

    ///  # potential(lhs, rhs)
    ///  lhs を基準とした rhs のポテンシャルを取得する
    ///
    ///  lhs, rhs が同じ連結成分に属しているのならば、上述のポテンシャル
    ///  そうでないならば nullopt
    ///  O(logn)
    Result potential(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        if (same(lhs, rhs)) {
            return Result(G::op(potential(rhs), G::inv(potential(lhs))));
        } else {
            return std::nullopt;
        }
    }

    ///  # same(lhs, rhs)
    ///  lhs, rhsが連結かどうか
    ///  O(log n)
    bool same(int lhs, int rhs) {
        assert(0 <= lhs && lhs < n_);
        assert(0 <= rhs && rhs < n_);
        return leader(lhs) == leader(rhs);
    }

    ///  # size(x)
    ///  頂点 x の属する連結成分の大きさを取得する
    ///  O(logn)
    int size(int x) {
        assert(0 <= x && x < n_);
        return -data_[leader(x)];
    }

    ///  # size()
    ///  O(1)
    int size() const {
        return n_;
    }

    ///  # groups()
    ///  連結成分分解する
    ///  返り値の頂点の順番は未定義
    ///  https://atcoder.github.io/ac-library/document_ja/dsu.html
    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(n_), group_size(n_);
        for (int i = 0; i < n_; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(n_);
        for (int i = 0; i < n_; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < n_; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(std::remove_if(result.begin(), result.end(), [&](const std::vector<int>& v) { return v.empty(); }),
                     result.end());
        return result;
    }

  private:
    int n_;
    ///  `data[i] < 0` ならば `i` を代表元とする連結成分の大きさ
    ///  `data[i] >= 0` ならば `i` の属する連結成分の代表元
    std::vector<int> data_;
    ///  頂点 `i` が代表元ならば `potential_[i] == 0`
    ///  そうでないならば、頂点`i` を基準としたポテンシャル
    ///  note:
    ///  常に正しい値が入っている訳ではない
    ///  適切に関数を呼び出して更新しないといけない
    std::vector<T> potential_;
};

}  //  namespace nono
