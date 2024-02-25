#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

namespace nono {

//  brief:
//  - 連結成分を管理する
template <class T>
class PotentializedUnionFind {
    static constexpr T INF = std::numeric_limits<T>::max();

  public:
    using value_type = T;

    PotentializedUnionFind() = default;
    PotentializedUnionFind(int size)
        : size_(size),
          data_(size, -1),
          potential_(size) {}

    //  brief:
    //  - 代表元を取得する
    int leader(int x) {
        assert(0 <= x && x < size_);
        if (data_[x] < 0) {
            return x;
        }
        potential_[x] += potential(data_[x]);
        return data_[x] = leader(data_[x]);
    }

    //  brief:
    //  - 頂点 `lhs` を基準とした頂点 `rhs` のポテンシャルが `diff`
    //  となるように辺を追加する
    //
    //  return:
    //  - 頂点 `lhs`, `rhs` がすでに連結ならば `flase`
    //  - そうでないならば `true`
    bool merge(int lhs, int rhs, T diff) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        if (same(lhs, rhs)) {
            return false;
        }
        diff += potential(lhs) - potential(rhs);
        lhs = leader(lhs);
        rhs = leader(rhs);
        //  size(lhs) > size(rhs)とする
        if (-data_[lhs] < -data_[lhs]) {
            std::swap(lhs, rhs);
            diff = -diff;
        }
        data_[lhs] += data_[rhs];
        data_[rhs] = lhs;
        potential_[rhs] = diff;

        return true;
    }

    //  brief:
    //  - `leader(x)` を基準とした `x` のポテンシャルを取得する
    T potential(int x) {
        assert(0 <= x && x < size_);
        leader(x);
        return potential_[x];
    }

    //  brief:
    //  - `lhs` を基準とした `rhs` のポテンシャルを取得する
    //
    //  return:
    //  - `lhs`, `rhs` が同じ連結成分に属しているのならば、上述のポテンシャル
    //  - そうでないならば `INF`
    T potential(int lhs, int rhs) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        if (same(lhs, rhs)) {
            return potential(rhs) - potential(lhs);
        } else {
            return INF;
        }
    }

    //  brief:
    //  - 頂点 `lhs`, `rhs`が連結かどうか
    bool same(int lhs, int rhs) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        return leader(lhs) == leader(rhs);
    }

    //  brief:
    //  - 頂点 `x` の属する連結成分の大きさを取得する
    int size(int x) {
        assert(0 <= x && x < size_);
        return -data_[leader(x)];
    }

    //  brief:
    //  - 頂点数を取得する
    int size() const {
        return size_;
    }

    //  brief:
    //  - 連結成分分解する
    //
    //  note:
    //  - 返り値の頂点の順番は未定義
    //
    //  see:
    //  - https://atcoder.github.io/ac-library/document_ja/dsu.html
    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(size_), group_size(size_);
        for (int i = 0; i < size_; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(size_);
        for (int i = 0; i < size_; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < size_; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(std::remove_if(
                         result.begin(), result.end(),
                         [&](const std::vector<int>& v) { return v.empty(); }),
                     result.end());
        return result;
    }

  private:
    int size_;
    //  brief::
    //  - `data[i] < 0` ならば `i` を代表元とする連結成分の大きさ
    //  - `data[i] >= 0` ならば `i` の属する連結成分の代表元
    std::vector<int> data_;
    //  brief:
    //  - 頂点 `i` が代表元ならば `potential_[i] == 0`
    //  - そうでないならば、頂点`i` を基準としたポテンシャル
    //
    //  note:
    //  - 常に正しい値が入っている訳ではない
    //  - 適切に関数を呼び出して更新しないといけない
    std::vector<T> potential_;
};

}  //  namespace nono
