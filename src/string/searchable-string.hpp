#pragma once

#include <string>
#include <vector>

#include "string/lcp-array.hpp"
#include "string/suffix-array.hpp"

namespace nono {

//  brief:
//  - 部分文字列の検索などが可能な文字列
//
//  details:
//  - Suffix ArrayとかLCP Arrayとかで頑張る
//
//  depend:
//  - Suffix Array
//  - LCP Array
class SearchableString {
  public:
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(N (log N) ^ 2)
    //
    //  note:
    //  - Suffix ArrayをSA-ISで構築すると線形になる
    SearchableString() = default;
    SearchableString(const std::string& str): data_(str), sa_(data_), lcp_(data_, sa_) {}
    SearchableString(std::string&& str): data_(str), sa_(data_), lcp_(data_, sa_) {}

    //  brief:
    //  - 部分文字列としてtargetを含むか
    //
    //  complexity:
    //  - O(M log N)
    bool contains(const std::string& target) const {
        int pos = lower_bound(target);
        if (pos == (int)sa_.size() || data_.compare(sa_[pos], target.size(), target) != 0) {
            return false;
        } else {
            return true;
        }
    }

    //  brief:
    //  - 部分文字列targetをprefixとして持つようなsuffixの
    //  - 開始位置を取得する
    //
    //  complexity:
    //  - O(N + M log N)
    std::vector<int> find(const std::string& target) const {
        int pos = lower_bound(target);
        if (pos == (int)sa_.size() || data_.compare(pos, target.size(), target) != 0) {
            return {};
        }
        std::vector<int> result;
        result.push_back(sa_[pos]);
        for (int i = pos; i < (int)lcp_.size(); i++) {
            if (lcp_[i] < (int)target.size()) {
                break;
            }
            result.push_back(sa_[i]);
        }
        return result;
    }

    const std::string& data() const {
        return data_;
    }

    const SuffixArray& suffix_array() const {
        return sa_;
    }

    const LCPArray& lcp_array() const {
        return lcp_;
    }

  private:
    int lower_bound(const std::string& target) const {
        int m = target.size();
        int ok = sa_.size();
        int ng = -1;
        while (ok - ng > 1) {
            int key = (ok + ng) / 2;
            if (data_.compare(sa_[key], m, target) < 0) {
                ng = key;
            } else {
                ok = key;
            }
        }
        return ok;
    }

    int upper_bound(const std::string& target) const {
        int m = target.size();
        int ok = sa_.size();
        int ng = -1;
        while (ok - ng > 1) {
            int key = (ok + ng) / 2;
            if (data_.compare(sa_[key], m, target) <= 0) {
                ng = key;
            } else {
                ok = key;
            }
        }
        return ok;
    }

    std::string data_;
    SuffixArray sa_;
    LCPArray lcp_;
};

}  //  namespace nono
