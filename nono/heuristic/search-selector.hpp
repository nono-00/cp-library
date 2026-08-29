#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <queue>
#include <vector>

namespace nono {

namespace search_selector_internal {

///  hash -> Value の開番地法テーブル(線形探索)。
///
///  std::unordered_mapを使わないのは、ビームサーチのホットパスだから。
///  ノードごとの確保・ポインタ追跡・素数剰余がいずれも効き、
///  幅1000・分岐4の条件で開番地法の2〜3倍の時間がかかる。
///
///  容量は2冪でマスクを取る。clear()は世代カウンタを進めるだけのO(1)。
///  hash値はZobrist等で既に十分ばらけている前提で、そのまま添字に使う
template <class Hash, class Value>
class HashTable {
  public:
    HashTable(): slots_(INITIAL_SLOTS), keys_(INITIAL_SLOTS), values_(INITIAL_SLOTS), stamps_(INITIAL_SLOTS, 0) {}

    ///  # clear()
    ///  O(1)。世代が一周したときだけO(容量)
    void clear() {
        size_ = 0;
        if (++generation_ == 0) {
            std::fill(stamps_.begin(), stamps_.end(), 0);
            generation_ = 1;
        }
    }

    ///  # probe(key, inserted)
    ///  keyに対応するスロット番号を返す。無ければ挿入してinsertedをtrueにする。
    ///  返したスロット番号は、次にprobe()を呼ぶまでの間だけ有効
    ///  ならしO(1)
    int probe(Hash key, bool& inserted) {
        //  挿入で使用率が1/2を超えるなら先に拡張する。
        //  返すスロット番号を無効化しないよう、probeより前に行う
        if ((size_ + 1) * 2 > slots_) grow_();
        int index = static_cast<int>(key) & (slots_ - 1);
        while (stamps_[index] == generation_ && keys_[index] != key) {
            index = (index + 1) & (slots_ - 1);
        }
        inserted = stamps_[index] != generation_;
        if (inserted) {
            stamps_[index] = generation_;
            keys_[index] = key;
            size_++;
        }
        return index;
    }

    ///  # find(key)
    ///  keyがあればスロット番号、無ければ-1。挿入も拡張もしない
    ///  ならしO(1)
    int find(Hash key) const {
        int index = static_cast<int>(key) & (slots_ - 1);
        while (stamps_[index] == generation_) {
            if (keys_[index] == key) return index;
            index = (index + 1) & (slots_ - 1);
        }
        return -1;
    }

    Value& value(int slot) {
        return values_[slot];
    }

    int size() const {
        return size_;
    }

  private:
    static constexpr int INITIAL_SLOTS = 1 << 10;

    void grow_() {
        std::vector<std::pair<Hash, Value>> live;
        live.reserve(size_);
        for (int i = 0; i < slots_; i++) {
            if (stamps_[i] == generation_) live.push_back({keys_[i], values_[i]});
        }
        slots_ *= 2;
        keys_.assign(slots_, Hash{});
        values_.assign(slots_, Value{});
        stamps_.assign(slots_, 0);
        generation_ = 1;
        size_ = 0;
        for (const auto& [key, value]: live) {
            int index = static_cast<int>(key) & (slots_ - 1);
            while (stamps_[index] == generation_) index = (index + 1) & (slots_ - 1);
            stamps_[index] = generation_;
            keys_[index] = key;
            values_[index] = value;
            size_++;
        }
    }

    int slots_ = INITIAL_SLOTS;
    int size_ = 0;
    unsigned generation_ = 1;
    std::vector<Hash> keys_;
    std::vector<Value> values_;
    std::vector<unsigned> stamps_;
};

}  //  namespace search_selector_internal

///  # BeamSelector
///  ビームサーチが1階層ごとに使う「重複排除つき上位width件の選択器」。
///
///  同じhashの候補は、スコアが最良のもの1件だけを残す。
///  「最初に来たものを残す」実装だと、後からより良い経路で同じ状態に到達しても
///  捨てられてしまい、探索が最初の到達順に固定される。
///
///  ## 使い方
///  1手ごとにclear() -> push()を候補の数だけ -> select(width)、を繰り返す。
///  select()の後にclear()を挟まずpush()してはいけない
template <class Hash>
class BeamSelector {
  public:
    struct Entry {
        double score;
        int id;
        Hash hash;
    };

    ///  # clear()
    ///  O(これまでにpushした候補数)
    void clear() {
        entries_.clear();
        index_.clear();
        selected_ = false;
    }

    ///  # push(id, hash, score)
    ///  候補を追加する。同じhashが既にあれば、スコアが良い方だけを残す
    ///  O(1)
    void push(int id, Hash hash, double score) {
        assert(!selected_);
        bool inserted;
        int slot = index_.probe(hash, inserted);
        if (inserted) {
            index_.value(slot) = static_cast<int>(entries_.size());
            entries_.push_back(Entry{score, id, hash});
        } else if (entries_[index_.value(slot)].score < score) {
            entries_[index_.value(slot)] = Entry{score, id, hash};
        }
    }

    ///  # select(width)
    ///  スコア上位width件を降順で返す。候補がwidth件未満ならすべて返す。
    ///  返した参照は次のclear()まで有効
    ///  O(N + width log width)
    const std::vector<Entry>& select(int width) {
        assert(width >= 0);
        selected_ = true;
        auto greater = [](const Entry& lhs, const Entry& rhs) {
            return lhs.score > rhs.score;
        };
        if (width < static_cast<int>(entries_.size())) {
            std::nth_element(entries_.begin(), entries_.begin() + width, entries_.end(), greater);
            entries_.resize(width);
        }
        std::sort(entries_.begin(), entries_.end(), greater);
        return entries_;
    }

    ///  # size()
    ///  重複排除後の候補数
    int size() const {
        return static_cast<int>(entries_.size());
    }

    bool empty() const {
        return entries_.empty();
    }

  private:
    std::vector<Entry> entries_;
    ///  hash -> entries_内の位置
    search_selector_internal::HashTable<Hash, int> index_;
    bool selected_ = false;
};

///  # BestFirstQueue
///  chokudaiサーチが1階層ごとに持つ「重複排除つき優先度付きキュー」。
///
///  BeamSelectorと同じく、同じhashについてはスコア最良のものだけを有効とする。
///  優先度付きキューは要素の更新ができないため、古くなった要素はキューに残したまま
///  取り出し時に読み飛ばす(遅延削除)。
template <class Hash>
class BestFirstQueue {
  public:
    ///  # push(id, hash, score)
    ///  O(log N)
    void push(int id, Hash hash, double score) {
        bool inserted;
        int slot = best_.probe(hash, inserted);
        if (!inserted && best_.value(slot).score >= score) return;
        best_.value(slot) = Best{score, id};
        heap_.push(Entry{score, id, hash});
    }

    ///  # empty()
    ///  ならしO(log N)
    bool empty() {
        drop_stale_();
        return heap_.empty();
    }

    ///  # top()
    ///  スコア最大のnode id
    ///  ならしO(log N)
    int top() {
        drop_stale_();
        assert(!heap_.empty());
        return heap_.top().id;
    }

    ///  # top_score()
    ///  ならしO(log N)
    double top_score() {
        drop_stale_();
        assert(!heap_.empty());
        return heap_.top().score;
    }

    ///  # pop()
    ///  ならしO(log N)
    void pop() {
        drop_stale_();
        assert(!heap_.empty());
        heap_.pop();
    }

    void clear() {
        heap_ = std::priority_queue<Entry>();
        best_.clear();
    }

  private:
    struct Entry {
        double score;
        int id;
        Hash hash;
        bool operator<(const Entry& other) const {
            return score < other.score;
        }
    };

    struct Best {
        double score;
        int id;
    };

    ///  同じhashについて、現在の最良と一致しない要素は無効
    void drop_stale_() {
        while (!heap_.empty()) {
            int slot = best_.find(heap_.top().hash);
            assert(slot >= 0);
            if (best_.value(slot).id == heap_.top().id) break;
            heap_.pop();
        }
    }

    std::priority_queue<Entry> heap_;
    search_selector_internal::HashTable<Hash, Best> best_;
};

}  //  namespace nono
