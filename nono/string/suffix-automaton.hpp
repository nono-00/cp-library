#pragma once

#include <cassert>
#include <map>
#include <ranges>
#include <type_traits>
#include <vector>

#ifdef DEBUG
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <optional>
#include <string>
#include <utility>
#endif

namespace nono {

///  brief : suffix automaton. 部分文字列の種類数をincrementalに処理できたりする. graphvizでの出力に対応.
template <class T = char>
class SuffixAutomaton {
    struct Node {
        Node(int len): len(len) {}
        int len;
        int link = -1;
        std::map<T, int> to;
    };

  public:
    SuffixAutomaton(): pos_(0), subseq_(0) {
        nodes_.emplace_back(0);
    }
    void add(T c) {
        seq_.push_back(c);
        int next_pos = nodes_.size();
        nodes_.emplace_back(nodes_[pos_].len + 1);
        int cur = pos_;
        while (cur != -1 && !nodes_[cur].to.contains(c)) {
            nodes_[cur].to[c] = next_pos;
            cur = nodes_[cur].link;
        }
        if (cur == -1) {
            //  [0, next_pos)の任意のsuffixは文字列にちょうど一回ずつ登場する
            nodes_[pos_ = next_pos].link = 0;
        } else if (nodes_[cur].len + 1 == nodes_[nodes_[cur].to[c]].len) {
            //  nodes[cur].to[c]に対応する文字列以上の長さを持つsuffixをインクリメント
            nodes_[pos_ = next_pos].link = nodes_[cur].to[c];
        } else {
            //  分割する
            int origin = nodes_[cur].to[c];
            int clone = nodes_.size();
            nodes_.emplace_back(nodes_[origin]);
            nodes_[clone].len = nodes_[cur].len + 1;
            nodes_[origin].link = nodes_[next_pos].link = clone;
            while (cur != -1 && nodes_[cur].to[c] == origin) {
                nodes_[cur].to[c] = clone;
                cur = nodes_[cur].link;
            }
            assert(cur == -1 || nodes_[cur].to[c] == nodes_[clone].link);
            pos_ = next_pos;
        }
        subseq_ += nodes_[pos_].len - nodes_[nodes_[pos_].link].len;
    }
    //  部分文字列の種類数
    long long subseq() {
        return subseq_;
    }
    //  not verified
    template <std::ranges::random_access_range R>
    bool match(const R& seq) {
        static_assert(std::is_same_v<typename R::value_type, T>);
        int pos = 0;
        for (auto c: seq) {
            if (!nodes_[pos].to.contains(c)) return false;
            pos = nodes_[pos].to[c];
        }
        return true;
    }

#ifdef DEBUG
    struct EdgeProperty {
        EdgeProperty(std::optional<T> label = std::nullopt, bool dotted = false): label(label), dotted(dotted) {}
        std::optional<T> label;
        bool dotted = false;
        friend std::ostream& operator<<(std::ostream& os, EdgeProperty property) {
            os << "[";
            if (property.label) os << "label=" << *(property.label) << ", ";
            os << "style=" << (property.dotted ? "\"dotted\"" : "\"solid\"") << ", ";
            os << "constraint=" << (property.dotted ? "false" : "true");
            os << "]";
            return os;
        }
    };
    struct VertexProperty {
        //  ets.
    };
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty, EdgeProperty>;
    template <class Graph>
    struct PropertyWriter {
        PropertyWriter(const Graph& g): graph_(g) {}
        template <class EdgeorVertex>
        void operator()(std::ostream& out, const EdgeorVertex& edge_or_vertex) const {
            out << graph_[edge_or_vertex];
        }

      private:
        const Graph& graph_;
    };

    //  普通の辺は実線で重み付きにして出力する
    //  linkは点線で重みなしで出力する
    //
    //  dot -Tpng test.dot -o test.png
    void write(std::string filename = "test.dot") {
        std::ofstream file(filename);
        std::vector<std::string> labels(nodes_.size());
        std::vector<std::pair<int, int>> edges;
        std::vector<EdgeProperty> propertys;
        std::vector<bool> used(nodes_.size());
        auto dfs = [&](auto&& self, int u, std::string label) -> void {
            used[u] = true;
            labels[u] = label;
            if (nodes_[u].link != -1) {
                edges.emplace_back(u, nodes_[u].link);
                propertys.emplace_back(std::nullopt, true);
            }
            for (auto [key, v]: nodes_[u].to) {
                edges.emplace_back(u, v);
                propertys.emplace_back(key, false);
                if (!used[v] && nodes_[u].len + 1 == nodes_[v].len) self(self, v, label + std::string(1, key));
            }
        };
        dfs(dfs, 0, "");
        Graph graph(edges.begin(), edges.end(), propertys.begin(), nodes_.size());
        boost::write_graphviz(file, graph, boost::make_label_writer(labels.data()), PropertyWriter(graph));
        std::cerr << "$ dot -Tpng test.dot -o test.png && open test.png" << std::endl;
    }
#else
    void write(std::string filename = "test.dot") {}
#endif

  private:
    int pos_;
    long long subseq_;
    std::vector<Node> nodes_;
    std::vector<T> seq_;
};

}  //  namespace nono
