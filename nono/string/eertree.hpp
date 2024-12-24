#pragma once

#include <algorithm>
#include <limits>
#include <map>
#include <numeric>
#include <ranges>
#include <type_traits>
#include <vector>

///  brief : 回文木. std::mapを使っているので, 少し遅い.
namespace nono {

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
    int size() {
        return nodes_.size() - 2;
    }
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
    template <std::ranges::random_access_range R>
    void add(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        for (auto c: seq) add(c);
    }

    //  node_id[i]: s[0:i+1]の最長接尾辞回文nodeid
    std::vector<int> node_id() {
        return node_id_;
    }

    std::vector<Node> nodes() {
        return nodes_;
    }

    //  freq[i]: node[i]に対応する連続文字列回文が何度出現するか
    //  sortしているので遅い
    //  verified: https://www.acmicpc.net/problem/10066
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
