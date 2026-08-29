// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <map>
#include <numeric>
#include <random>
#include <vector>

#include "nono/heuristic/search-selector.hpp"

namespace nono {

using Hash = unsigned long long;

//  ---------------- BeamSelector ----------------

///  hashごとにスコア最良を残し、上位widthを降順に並べる素朴な実装。
///  同スコアのときは先にpushされた方を残す(BeamSelectorと同じ規則)
struct NaiveBeam {
    struct Entry {
        double score;
        int id;
        Hash hash;
    };
    std::vector<Entry> entries;
    std::map<Hash, int> index;

    void push(int id, Hash hash, double score) {
        auto it = index.find(hash);
        if (it == index.end()) {
            index[hash] = entries.size();
            entries.push_back({score, id, hash});
        } else if (entries[it->second].score < score) {
            entries[it->second] = {score, id, hash};
        }
    }

    std::vector<Entry> select(int width) {
        auto result = entries;
        std::stable_sort(result.begin(), result.end(),
                         [](const Entry& l, const Entry& r) { return l.score > r.score; });
        if ((int)result.size() > width) result.resize(width);
        return result;
    }
};

///  スコアをすべて相異なるようにして、順序を一意に定める
void test_beam_selector(unsigned seed, int candidates, int distinct_hashes, int width) {
    std::mt19937 mt(seed);
    std::vector<double> scores(candidates);
    std::iota(scores.begin(), scores.end(), 0.0);
    std::shuffle(scores.begin(), scores.end(), mt);
    std::uniform_int_distribution<int> hash(0, distinct_hashes - 1);

    BeamSelector<Hash> selector;
    NaiveBeam naive;
    for (int id = 0; id < candidates; id++) {
        Hash h = hash(mt);
        selector.push(id, h, scores[id]);
        naive.push(id, h, scores[id]);
    }
    assert(selector.size() == (int)naive.entries.size());

    auto expected = naive.select(width);
    const auto& actual = selector.select(width);
    assert(actual.size() == expected.size());
    for (std::size_t i = 0; i < actual.size(); i++) {
        assert(actual[i].id == expected[i].id);
        assert(actual[i].score == expected[i].score);
        assert(actual[i].hash == expected[i].hash);
    }
    //  降順であること
    for (std::size_t i = 1; i < actual.size(); i++) assert(actual[i - 1].score >= actual[i].score);
    //  hashが重複しないこと
    std::vector<Hash> hashes;
    for (const auto& entry: actual) hashes.push_back(entry.hash);
    std::sort(hashes.begin(), hashes.end());
    assert(std::unique(hashes.begin(), hashes.end()) == hashes.end());
}

///  push順に関わらず、同じhashではスコア最良が残ること
void test_beam_selector_keeps_best() {
    for (int order = 0; order < 2; order++) {
        BeamSelector<Hash> selector;
        if (order == 0) {
            selector.push(10, 777, 1.0);
            selector.push(20, 777, 5.0);
        } else {
            selector.push(20, 777, 5.0);
            selector.push(10, 777, 1.0);
        }
        const auto& selected = selector.select(10);
        assert(selected.size() == 1);
        assert(selected[0].id == 20);
        assert(selected[0].score == 5.0);
    }
    //  clear()で再利用できること
    BeamSelector<Hash> selector;
    selector.push(1, 1, 1.0);
    selector.select(10);
    selector.clear();
    assert(selector.empty());
    selector.push(2, 1, 2.0);
    const auto& selected = selector.select(10);
    assert(selected.size() == 1 && selected[0].id == 2);
    //  widthが候補数以上/0のとき
    BeamSelector<Hash> other;
    for (int i = 0; i < 5; i++) other.push(i, i, i);
    assert(other.select(100).size() == 5);
    other.clear();
    for (int i = 0; i < 5; i++) other.push(i, i, i);
    assert(other.select(0).empty());
}

//  ---------------- BestFirstQueue ----------------

///  有効な要素(hashごとの最良と一致するもの)から最大スコアを線形に探す素朴な実装
struct NaiveQueue {
    struct Entry {
        double score;
        int id;
        Hash hash;
    };
    std::vector<Entry> entries;
    std::map<Hash, std::pair<double, int>> best;

    void push(int id, Hash hash, double score) {
        auto it = best.find(hash);
        if (it != best.end()) {
            if (it->second.first >= score) return;
            it->second = {score, id};
        } else {
            best[hash] = {score, id};
        }
        entries.push_back({score, id, hash});
    }

    int find_top() const {
        int result = -1;
        for (int i = 0; i < (int)entries.size(); i++) {
            if (best.at(entries[i].hash).second != entries[i].id) continue;
            if (result == -1 || entries[result].score < entries[i].score) result = i;
        }
        return result;
    }

    bool empty() const {
        return find_top() == -1;
    }
    int top() const {
        return entries[find_top()].id;
    }
    void pop() {
        entries.erase(entries.begin() + find_top());
    }
};

void test_best_first_queue(unsigned seed, int operations, int distinct_hashes) {
    std::mt19937 mt(seed);
    std::vector<double> scores(operations);
    std::iota(scores.begin(), scores.end(), 0.0);
    std::shuffle(scores.begin(), scores.end(), mt);
    std::uniform_int_distribution<int> hash(0, distinct_hashes - 1), coin(0, 2);

    BestFirstQueue<Hash> queue;
    NaiveQueue naive;
    //  idはpushしたときだけ増えるので、ループ添字とは一致しない
    std::vector<double> score_of_id;
    for (int i = 0; i < operations; i++) {
        assert(queue.empty() == naive.empty());
        if (coin(mt) == 0 && !naive.empty()) {
            assert(queue.top() == naive.top());
            queue.pop();
            naive.pop();
        } else {
            Hash h = hash(mt);
            int id = (int)score_of_id.size();
            score_of_id.push_back(scores[i]);
            queue.push(id, h, scores[i]);
            naive.push(id, h, scores[i]);
        }
    }
    //  残りを全て取り出して順序が一致すること
    while (!naive.empty()) {
        assert(!queue.empty());
        assert(queue.top() == naive.top());
        assert(queue.top_score() == score_of_id[naive.top()]);
        queue.pop();
        naive.pop();
    }
    assert(queue.empty());
}

void test_best_first_queue_semantics() {
    BestFirstQueue<Hash> queue;
    assert(queue.empty());

    //  同じhashではスコア最良だけが残る
    queue.push(1, 100, 3.0);
    queue.push(2, 100, 1.0);
    assert(queue.top() == 1);
    queue.push(3, 100, 9.0);
    assert(queue.top() == 3);
    queue.pop();
    //  最良を取り出した後、それより悪い同hashの要素は復活しない
    assert(queue.empty());
    queue.push(4, 100, 5.0);
    assert(queue.empty());
    //  より良いものは入る
    queue.push(5, 100, 12.0);
    assert(queue.top() == 5);

    queue.clear();
    assert(queue.empty());
    queue.push(6, 100, 1.0);
    assert(queue.top() == 6);
}

void solve() {
    test_beam_selector(1, 1000, 50, 10);
    test_beam_selector(2, 1000, 1000, 100);
    test_beam_selector(3, 500, 3, 10);
    test_beam_selector(4, 200, 200, 500);
    test_beam_selector(5, 1, 1, 1);
    test_beam_selector_keeps_best();

    test_best_first_queue(1, 2000, 30);
    test_best_first_queue(2, 2000, 500);
    test_best_first_queue(3, 1000, 2);
    test_best_first_queue_semantics();
}

}  //  namespace nono

int main() {
    nono::solve();
}
