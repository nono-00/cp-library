#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <ranges>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

///  brief : アホコラ. std::mapを使っているので遅め. 配列に書き換えると速くなる.
namespace nono {
class AhoCorasick {
    struct Node {
        Node(int len): len(len) {
            //  to.fill(-1);
        }
        int len = 0;
        //  suffix link.
        //  最長共通接尾辞に移動する
        int link = 0;
        //  共通のsuffixを持つものの中で, 最も近いend of word.
        int closest_end = 0;
        std::map<char, int> to;
        //  std::array<int, 26> to;
    };

  public:
    AhoCorasick(): pos_(0) {}
    template <std::ranges::random_access_range R>
    AhoCorasick(const std::vector<R>& words): pos_(0),
                                              nodes_(1, Node(0)) {
        static_assert(std::is_same_v<typename R::value_type, char>);
        for (const auto& word: words) {
            for (auto c: word) add(c);
            nodes_[pos_].closest_end = pos_;
            pos_ = 0;
        }
        std::queue<int> que;
        que.push(0);
        while (!que.empty()) {
            int u = que.front();
            que.pop();
            //  for (int c = 0; c < 26; c++) {
            //      if (nodes_[u].to[c] == -1) continue;
            //      assert(0 <= c && c < 26);
            //      int v = nodes_[u].to[c];
            //      que.push(v);
            //      int link = nodes_[u].link;
            //      while (link != 0 && nodes_[link].to[c] == -1) link = nodes_[link].link;
            //      nodes_[v].link = (u == 0 || nodes_[link].to[c] == -1 ? 0 : nodes_[link].to[c]);
            //      if (nodes_[v].closest_end == 0) nodes_[v].closest_end = nodes_[nodes_[v].link].closest_end;
            //  }
            for (auto [c, v]: nodes_[u].to) {
                que.push(v);
                int link = nodes_[u].link;
                while (link != 0 && !nodes_[link].to.contains(c)) link = nodes_[link].link;
                nodes_[v].link = (u == 0 || !nodes_[link].to.contains(c) ? 0 : nodes_[link].to[c]);
                if (nodes_[v].closest_end == 0) nodes_[v].closest_end = nodes_[nodes_[v].link].closest_end;
            }
        }
    }
    //  result[i]: seq[0:i+1]のsuffixと一致する単語のなかで最も長いもの
    template <std::ranges::random_access_range R>
    std::vector<int> longest_suffix(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, char>);
        int n = std::ssize(seq);
        std::vector<int> result(n);
        for (int i = 0; i < n; i++) {
            //  auto c = seq[i] - 'a';
            //  assert(0 <= c && c < 26);
            //  while (true) {
            //      if (nodes_[pos_].to[c] != -1) {
            //          pos_ = nodes_[pos_].to[c];
            //          break;
            //      }
            //      if (pos_ == 0) break;
            //      pos_ = nodes_[pos_].link;
            //  }
            //  result[i] = nodes_[nodes_[pos_].closest_end].len;
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

    std::vector<Node> nodes() const {
        return nodes_;
    }

  private:
    void add(char c) {
        //  if (nodes_[pos_].to[c - 'a'] == -1) {
        //      nodes_[pos_].to[c - 'a'] = nodes_.size();
        //      nodes_.emplace_back(nodes_[pos_].len + 1);
        //  }
        //  pos_ = nodes_[pos_].to[c - 'a'];
        if (!nodes_[pos_].to.contains(c)) {
            nodes_[pos_].to[c] = nodes_.size();
            nodes_.emplace_back(nodes_[pos_].len + 1);
        }
        pos_ = nodes_[pos_].to[c];
    }
    int pos_;
    std::vector<Node> nodes_;
};
}  //  namespace nono

namespace nono {

void solve() {
    int n;
    std::string s;
    std::cin >> n >> s;
    int m;
    std::cin >> m;
    std::vector<std::string> words(m);
    for (auto& word: words) std::cin >> word;
    AhoCorasick aho(words);
    auto len = aho.longest_suffix(s);
    std::vector<int> imos(n + 1);
    for (int i = 0; i < n; i++) {
        imos[i + 1 - len[i]]++;
        imos[i + 1]--;
    }
    for (int i = 0; i < n; i++) imos[i + 1] += imos[i];
    std::cout << std::count(imos.begin(), imos.end() - 1, 0) << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
