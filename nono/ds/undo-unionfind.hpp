#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace nono {

//  # UndoUnionFind
//  直前のmergeを取り消せるUnionFind。辺追加クエリをstackで管理するような感じ
//  頂点が [0, size()) でないと壊れる
class UndoUnionFind {
    struct History {
        int lhs;
        int rhs;
        int size;
    };

  public:
    UndoUnionFind() = default;

    //  # UndoUnionFind(size)
    //  頂点数がsizeで、全ての頂点が別々のグループにある状態を作る
    UndoUnionFind(int size): size_(size), data_(size, -1) {}

    //  # leader(x)
    //  xが属するグループの代表元を返す
    //  O(log n)
    int leader(int x) {
        assert(0 <= x && x < size_);
        if (data_[x] < 0) {
            return x;
        } else {
            return leader(data_[x]);
        }
    }

    //  # merge(lhs, rhs)
    //  lhsのグループとrhsのグループを併合する。edges <- edges + [(lhs, rhs)]
    //  もともと同じグループならfalse、併合したらtrueを返す。どちらの場合もundo()の対象になる
    //  O(log n)
    bool merge(int lhs, int rhs) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        lhs = leader(lhs);
        rhs = leader(rhs);
        if (-data_[lhs] < -data_[rhs]) {
            std::swap(lhs, rhs);
        }
        historys_.emplace_back(lhs, rhs, data_[rhs]);
        if (lhs == rhs) return false;
        data_[lhs] += data_[rhs];
        data_[rhs] = lhs;
        return true;
    }

    //  # undo()
    //  直前のmerge()を取り消す。edges <- edges[:-1]
    //  merge()を呼んだ回数より多く呼ぶと壊れる
    //  O(1)
    void undo() {
        assert(!historys_.empty());
        auto [lhs, rhs, data] = historys_.back();
        historys_.pop_back();
        if (lhs != rhs) {
            data_[rhs] = data;
            data_[lhs] -= data;
        }
    }

    //  # same(lhs, rhs)
    //  lhsとrhsが同じグループか
    //  O(log n)
    bool same(int lhs, int rhs) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        return (leader(lhs) == leader(rhs));
    }

    //  # size(x)
    //  xが属するグループの大きさ
    //  O(log n)
    int size(int x) {
        assert(0 <= x && x < size_);
        return -data_[leader(x)];
    }

    //  # size()
    //  頂点数
    //  O(1)
    int size() const {
        return size_;
    }

    //  # groups()
    //  グループごとの頂点の列を返す。グループの順序と、グループ内の頂点の順序は未定義
    //  https://atcoder.github.io/ac-library/document_ja/dsu.html
    //  O(n log n)
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
        result.erase(std::remove_if(result.begin(), result.end(), [&](const std::vector<int>& v) { return v.empty(); }),
                     result.end());
        return result;
    }

  private:
    int size_;
    std::vector<int> data_;
    std::vector<History> historys_;
};

}  //  namespace nono
