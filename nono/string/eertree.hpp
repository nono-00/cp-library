#pragma once

#include <map>
#include <vector>

/// brief : 回文木. 文字列を突っ込むと連続文字列回文の個数を求めることができる.
namespace nono {

template <class T = char>
class Eertree {
    struct Node {
        Node(int id, int len, int parent, int link): id(id), len(len), parent(parent), link(link) {}
        int id;
        int len;
        int parent;
        int link;
        std::map<T, int> to;
    };

  public:
    Eertree(): pos_(-1) {
        nodes_.emplace_back(-1, -1, 0, -1);
        nodes_.emplace_back(0, 0, 0, -1);
    }
    int size() {
        return n_;
    }
    void add(T c) {
        const int i = seq_.size();
        seq_.push_back(c);
        while (i - nodes_[pos_ + 1].len - 1 < 0 || seq_[i] != seq_[i - nodes_[pos_ + 1].len - 1])
            pos_ = nodes_[pos_ + 1].link;
        if (!nodes_[pos_ + 1].to.contains(c)) {
            nodes_[pos_ + 1].to[c] = ++n_;
            int link = nodes_[pos_ + 1].link;
            while (i - nodes_[link + 1].len - 1 < 0 || seq_[i] != seq_[i - nodes_[link + 1].len - 1])
                link = nodes_[link + 1].link;
            nodes_.emplace_back(n_, nodes_[pos_ + 1].len + 2, pos_, (pos_ != link ? nodes_[link + 1].to[c] : 0));
        }
        pos_ = nodes_[pos_ + 1].to[c];
        node_id_.push_back(pos_);
    }
    void add(const std::vector<T>& s) {
        for (auto c: s) add(c);
    }

    //  node_id[i]: s[0:i+1]の最長接尾辞回文nodeid
    std::vector<int> node_id() {
        return node_id_;
    }

    //  nodes[0]: even
    //  nodes[i]: node i
    std::vector<Node> nodes() {
        return std::vector<Node>{nodes_.begin() + 1, nodes_.end()};
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
