#pragma once

#include <cassert>
#include <map>
#include <queue>
#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <vector>

///  # Aho Corasick
namespace nono {
template <class T = char>
class AhoCorasick {
    struct Node {
        Node(int len, int parent): len(len), parent(parent) {}
        ///  根から何回移動したか
        int len = 0;
        ///  親頂点
        int parent = 0;
        ///  suffix link.
        ///  最長共通接尾辞に移動する
        int link = 0;
        ///  単語の終端となる個数
        int count = 0;
        ///  共通のsuffixを持つものの中で, 最も近いend of word.
        int closest_end = 0;
        std::map<T, int> to;
    };

  public:
    AhoCorasick(): pos_(0) {}
    ///  # AhoCorasick
    ///  O(sum[word in words](|word|))
    template <std::ranges::random_access_range R>
    AhoCorasick(const std::vector<R>& words): pos_(0),
                                              nodes_(1, Node(0, 0)) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        for (const auto& word: words) {
            for (auto c: word) add(c);
            nodes_[pos_].count++;
            nodes_[pos_].closest_end = pos_;
            pos_ = 0;
        }
        std::queue<int> que;
        que.push(0);
        while (!que.empty()) {
            int u = que.front();
            que.pop();
            for (auto [c, v]: nodes_[u].to) {
                que.push(v);
                int link = nodes_[u].link;
                while (link != 0 && !nodes_[link].to.contains(c)) link = nodes_[link].link;
                nodes_[v].link = (u == 0 || !nodes_[link].to.contains(c) ? 0 : nodes_[link].to[c]);
                nodes_[v].count += nodes_[nodes_[v].link].count;
                if (nodes_[v].closest_end == 0) nodes_[v].closest_end = nodes_[nodes_[v].link].closest_end;
            }
        }
    }
    ///  # contains(seq)
    ///  exists word in words, word is continuous subseq of input seq.
    ///  verified : https://www.acmicpc.net/problem/9250
    ///  O(|seq|)
    template <std::ranges::random_access_range R>
    bool contains(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        pos_ = 0;
        for (auto c: seq) {
            while (true) {
                if (nodes_[pos_].to.contains(c)) {
                    pos_ = nodes_[pos_].to[c];
                    break;
                }
                if (pos_ == 0) break;
                pos_ = nodes_[pos_].link;
            }
            if (nodes_[pos_].count) return true;
        }
        return false;
    }

    ///  sum[word in words](freq of word in seq)
    ///  verified : https://www.acmicpc.net/problem/10256
    ///  O(|seq|)
    template <class U, std::ranges::random_access_range R>
    U count(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        U result = 0;
        pos_ = 0;
        for (auto c: seq) {
            while (true) {
                if (nodes_[pos_].to.contains(c)) {
                    pos_ = nodes_[pos_].to[c];
                    break;
                }
                if (pos_ == 0) break;
                pos_ = nodes_[pos_].link;
            }
            result += nodes_[pos_].count;
        }
        return result;
    }

    ///  # longest suffix
    ///  result[i] : seq[0:i+1]のsuffixと一致する単語のなかで最も長いwordの長さ
    ///  verified : https://www.acmicpc.net/problem/2809
    ///  O(|seq|)
    template <std::ranges::random_access_range R>
    std::vector<int> longest_suffix(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        int n = std::ssize(seq);
        std::vector<int> result(n);
        for (int i = 0; i < n; i++) {
            auto c = seq[i];
            while (true) {
                if (nodes_[pos_].to.contains(c)) {
                    pos_ = nodes_[pos_].to[c];
                    break;
                }
                if (pos_ == 0) break;
                pos_ = nodes_[pos_].link;
            }
            result[i] = nodes_[nodes_[pos_].closest_end].len;
        }
        return result;
    }

    ///  # nodes()
    std::vector<Node> nodes() const {
        return nodes_;
    }

  private:
    void add(T c) {
        if (!nodes_[pos_].to.contains(c)) {
            nodes_[pos_].to[c] = nodes_.size();
            nodes_.emplace_back(nodes_[pos_].len + 1, pos_);
        }
        pos_ = nodes_[pos_].to[c];
    }
    int pos_;
    std::vector<Node> nodes_;
};
}  //  namespace nono
