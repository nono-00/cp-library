#pragma once

#include <algorithm>
#include <limits>
#include <map>
#include <numeric>
#include <ranges>
#include <type_traits>
#include <vector>

namespace nono {

//  # Eertree
//  回文木(palindromic tree)。列に連続部分列として現れる相異なる回文を、1つずつノードとして持つ
//  Tの最大値を番兵に使うので、列に std::numeric_limits<T>::max() があると壊れる
//  以下の σ は文字の種類数
template <class T = char>
class Eertree {
    struct Node {
        Node(int len, int parent = 0, int link = 0): len(len), parent(parent), link(link) {}
        //  長さ
        int len;
        //  親頂点
        int parent;
        //  suffix link
        int link;
        std::map<T, int> to;
    };

  public:
    Eertree(): pos_(0) {
        nodes_.emplace_back(Node(-1));
        nodes_.emplace_back(Node(0));
        seq_.push_back(std::numeric_limits<T>::max());
    }

    //  # size()
    //  相異なる回文の個数。長さ-1と長さ0の2つの根は数えない
    //  O(1)
    int size() {
        return nodes_.size() - 2;
    }

    //  # add(c)
    //  列の末尾にcを追加する
    //  ならし O(log σ)
    void add(T c) {
        const int i = seq_.size();
        seq_.push_back(c);
        pos_ = go(pos_, i);
        if (!nodes_[pos_].to.contains(c)) {
            nodes_[pos_].to[c] = nodes_.size();
            int link = pos_ == 0 ? 1 : nodes_[go(nodes_[pos_].link, i)].to[c];
            nodes_.emplace_back(nodes_[pos_].len + 2, pos_, link);
        }
        pos_ = nodes_[pos_].to[c];
        node_id_.push_back(pos_);
    }

    //  # add(seq)
    //  列の末尾にseqを追加する
    //  O(|seq| log σ)
    template <std::ranges::random_access_range R>
    void add(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        for (auto c: seq) add(c);
    }

    //  # node_id()
    //  node_id[i]: s[0:i+1]の最長接尾辞回文nodeid
    //  O(n)
    std::vector<int> node_id() {
        return node_id_;
    }

    //  # nodes()
    //  ノード列を返す。0番が長さ-1の根、1番が長さ0の根
    //  O(n)
    std::vector<Node> nodes() {
        return nodes_;
    }

    //  # freq()
    //  freq[i]: node[i]に対応する回文が、連続部分列として何度出現するか
    //  sortしているので遅い
    //  verified: https://www.acmicpc.net/problem/10066
    //  O(n log n)
    std::vector<int> freq() {
        std::vector<int> ids(nodes_.size());
        std::iota(ids.begin(), ids.end(), 0);
        std::sort(ids.begin(), ids.end(), [&](int lhs, int rhs) { return nodes_[lhs].len > nodes_[rhs].len; });
        std::vector<int> result(nodes_.size());
        for (auto id: node_id_) result[id]++;
        for (auto id: ids) {
            result[nodes_[id].link] += result[id];
        }
        return result;
    }

  private:
    bool fail(int pos, int i) {
        return seq_[i] != seq_[i - nodes_[pos].len - 1];
    }

    int go(int pos, int i) {
        while (fail(pos, i)) pos = nodes_[pos].link;
        return pos;
    }

    int pos_;
    //  odd, even 除いた頂点数
    std::vector<T> seq_;
    std::vector<int> node_id_;
    std::vector<Node> nodes_;
};

}  //  namespace nono
