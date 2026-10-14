#pragma once

#include <cassert>
#include <string>
#include <vector>

namespace nono {

//  # lcp_array(s, sa)
//  LCP配列。result[i] は、接尾辞 s[sa[i]:] と s[sa[i + 1]:] の最長共通接頭辞の長さ
//  長さは |s| - 1
//  based on <https://atcoder.github.io/ac-library/master/document_ja/string.html>
//  Reference:
//  T. Kasai, G. Lee, H. Arimura, S. Arikawa, and K. Park,
//  Linear-Time Longest-Common-Prefix Computation in Suffix Arrays and Its
//  Applications
//  sが空だと壊れる。saがsの接尾辞配列でないと壊れる
//  O(|s|)
template <class T>
std::vector<int> lcp_array(const std::vector<T>& s, const std::vector<int>& sa) {
    int n = int(s.size());
    assert(n >= 1);
    std::vector<int> rnk(n);
    for (int i = 0; i < n; i++) {
        rnk[sa[i]] = i;
    }
    std::vector<int> lcp(n - 1);
    int h = 0;
    for (int i = 0; i < n; i++) {
        if (h > 0) h--;
        if (rnk[i] == 0) continue;
        int j = sa[rnk[i] - 1];
        for (; j + h < n && i + h < n; h++) {
            if (s[j + h] != s[i + h]) break;
        }
        lcp[rnk[i] - 1] = h;
    }
    return lcp;
}

//  # lcp_array(s, sa)
//  std::string版。内容は同じ
std::vector<int> lcp_array(const std::string& s, const std::vector<int>& sa) {
    int n = int(s.size());
    std::vector<int> s2(n);
    for (int i = 0; i < n; i++) {
        s2[i] = s[i];
    }
    return lcp_array(s2, sa);
}

}  //  namespace nono
