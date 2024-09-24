#pragma once

#include <algorithm>
#include <map>
#include <numeric>
#include <utility>
#include <vector>

///  brief : 回文木. nodeへのアクセス時に-1しないと壊れるかも？
namespace nono {

//  odd : id = 0
//  even : id = 1
template <class T = char>
class Eertree {
    struct Node {
        Node(int id, int len, int parent, int link, int right)
            : id(id),
              len(len),
              parent(parent),
              link(link),
              right(right) {}
        //  頂点番号 最初の回文は2
        int id;
        //  長さ
        int len;
        //  親頂点
        int parent;
        //  suffix link
        int link;
        //  対応する連続文字列回文が最初に登場する最右要素
        int right;
        std::map<T, int> to;
    };

  public:
    Eertree(): pos_(0) {
        nodes_.emplace_back(0, -1, 0, 0, -1);
        nodes_.emplace_back(1, 0, 0, 0, -1);
    }
    int size() {
        return n_;
    }
    void add(T c) {
        const int i = seq_.size();
        seq_.push_back(c);
        while (i - nodes_[pos_].len - 1 < 0 || seq_[i] != seq_[i - nodes_[pos_].len - 1]) pos_ = nodes_[pos_].link;
        if (!nodes_[pos_].to.contains(c)) {
            n_++;
            nodes_[pos_].to[c] = nodes_.size();
            int link = nodes_[pos_].link;
            while (i - nodes_[link].len - 1 < 0 || seq_[i] != seq_[i - nodes_[link].len - 1]) link = nodes_[link].link;
            nodes_.emplace_back(nodes_.size(), nodes_[pos_].len + 2, pos_, (pos_ != link ? nodes_[link].to[c] : 1),
                                i + 1);
        }
        pos_ = nodes_[pos_].to[c];
        node_id_.push_back(pos_);
    }
    void add(const std::vector<T>& s) {
        for (auto c: s) add(c);
    }

    //  node_id[i]: s[0:i+1]の最長接尾辞回文nodeid
    std::vector<int> node_id() {
        return node_id_;
    }

    std::vector<Node> nodes() {
        return nodes_;
    }

    //  freq[i]: node[i]に対応する連続文字列回文が何度出現するか
    //  verified: https://www.acmicpc.net/problem/10066
    std::vector<int> freq() {
        std::vector<int> ids(n_ + 2);
        std::iota(ids.begin(), ids.end(), 0);
        sort(ids.begin(), ids.end(), [&](int lhs, int rhs) { return nodes_[lhs + 1].len > nodes_[rhs + 1].len; });
        std::vector<int> result(n_ + 2);
        for (auto id: node_id_) result[id]++;
        for (auto id: ids) {
            if (id <= 1) continue;
            if (nodes_[id].link >= 0) {
                result[nodes_[id].link] += result[id];
            }
        }
        return result;
    }

  private:
    int pos_;
    //  odd, even 除いた頂点数
    int n_ = 0;
    std::vector<T> seq_;
    std::vector<int> node_id_;
    std::vector<Node> nodes_;
};

}  //  namespace nono
