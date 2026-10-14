#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace nono {

//  # SearchTree
//  ビームサーチ・chokudaiサーチが共通で必要とする「根からの操作列を復元できる木」。
//  ノードはActionとparentだけを持ち、Stateもスコアも持たない。
//  それらは探索方針の側(選択器・ドライバ)が別に管理する。
//  各メンバ関数に渡すidが、この木のノード(contains(id)がtrue)でないと壊れる。
//
//  ## なぜStateを持たないか
//  ノードごとにStateを丸ごと抱えると、メモリがO(幅 * 深さ * sizeof(State))になる。
//  幅1000・深さ1000・State 1KiBで約1GiBに達し、AHCでは先にメモリが壊れる。
//  Actionだけなら同じ条件で数MiBに収まり、可変なStateを1つ使い回して
//  move_state()で移動すればコピーも発生しない。
//  Stateを持たせたい場合は、node idを添字とする配列を呼び出し側で別に持てばよい。
//
//  ## Actionにはundoに必要な情報を持たせる
//  Actionは値で保持されるので、「遷移で壊れるが復元が高価な値」は
//  Actionに退避しておくとundoが軽くなる。
//  例えば「配列のある要素を書き換える」遷移なら、書き換え前の値をActionに入れておく。
//  Actionが多少大きくなっても、Stateを丸ごと持つより遥かに小さい。
//
//  ## メモリ
//  ノードは再利用しない。size()個のノードを保持し続けるので、
//  「1手あたりの候補数 * 手数」が大きい問題ではreserve()で確保しておく。
template <class Action>
class SearchTree {
  public:
    //  # ROOT
    //  根のid。根はActionを持たない
    static constexpr int ROOT = 0;

    //  # SearchTree()
    //  根だけの木を作る
    SearchTree() {
        nodes_.push_back(Node{Action{}, -1, 0});
    }

    //  # reserve(capacity)
    //  ノードcapacity個ぶんの領域を先に確保する
    void reserve(std::size_t capacity) {
        nodes_.reserve(capacity);
    }

    //  # add(parent, action)
    //  parentの子としてactionを追加し、そのidを返す
    //  O(1)
    int add(int parent, const Action& action) {
        assert(contains(parent));
        nodes_.push_back(Node{action, parent, nodes_[parent].depth + 1});
        return static_cast<int>(nodes_.size()) - 1;
    }

    //  # parent(id)
    //  根に対しては-1
    int parent(int id) const {
        assert(contains(id));
        return nodes_[id].parent;
    }

    //  # action(id)
    //  parent(id)からidへ至るAction。根に対しては呼べない
    const Action& action(int id) const {
        assert(contains(id) && id != ROOT);
        return nodes_[id].action;
    }

    //  # depth(id)
    //  根からの辺数
    int depth(int id) const {
        assert(contains(id));
        return nodes_[id].depth;
    }

    //  # size()
    //  根を含むノード数
    int size() const {
        return static_cast<int>(nodes_.size());
    }

    //  # contains(id)
    //  idがこの木のノードか (0 <= id < size())
    bool contains(int id) const {
        return 0 <= id && id < size();
    }

    //  # path(id)
    //  根からidに至るAction列
    //  O(depth(id))
    std::vector<Action> path(int id) const {
        std::vector<Action> result;
        path(id, result);
        return result;
    }

    //  # path(id, out)
    //  outを上書きする。確保を避けたい場合に使う
    //  O(depth(id))
    void path(int id, std::vector<Action>& out) const {
        assert(contains(id));
        out.clear();
        while (id != ROOT) {
            out.push_back(nodes_[id].action);
            id = nodes_[id].parent;
        }
        std::reverse(out.begin(), out.end());
    }

    //  # move_state(state, from, to, apply, undo)
    //  stateがfromに対応する状態であるとき、toに対応する状態へ移す。
    //  共通祖先までundoで戻り、そこからapplyで下る。
    //  applyはapply(state, action)、undoはundo(state, action)で、互いに逆でないと壊れる
    //  O(fromとtoの距離)
    //
    //  ビームサーチのように「1階層ぶんのノードを決まった順で全て辿る」場合は、
    //  各ノードでこれを呼ぶ代わりに、階層全体の走査順(Euler tour)を
    //  平坦な配列として先に作った方が速い。親を辿るポインタ追跡が
    //  連続アクセスに変わるため。ここでは任意の2点間を移動できる汎用の形にしてある
    template <class State, class Apply, class Undo>
    void move_state(State& state, int from, int to, Apply&& apply, Undo&& undo) {
        assert(contains(from) && contains(to));
        descent_.clear();
        while (nodes_[from].depth > nodes_[to].depth) {
            undo(state, nodes_[from].action);
            from = nodes_[from].parent;
        }
        while (nodes_[to].depth > nodes_[from].depth) {
            descent_.push_back(to);
            to = nodes_[to].parent;
        }
        while (from != to) {
            undo(state, nodes_[from].action);
            from = nodes_[from].parent;
            descent_.push_back(to);
            to = nodes_[to].parent;
        }
        for (auto it = descent_.rbegin(); it != descent_.rend(); ++it) {
            apply(state, nodes_[*it].action);
        }
    }

  private:
    struct Node {
        Action action;
        int parent;
        int depth;
    };

    std::vector<Node> nodes_;
    //  move_stateで下りに使う作業領域。呼び出しごとの確保を避けるために持つ
    std::vector<int> descent_;
};

}  //  namespace nono
