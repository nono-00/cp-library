#pragma once

#include <bit>
#include <iterator>
#include <map>
#include <vector>

#include "nono/data-structure/undo-unionfind.hpp"

namespace nono {

template <class F>
void offline_dynamic_connectivity(int n, const std::vector<std::pair<int, int>>& querys, F func) {
    int t = std::bit_ceil(querys.size());
    std::vector<std::vector<std::pair<int, int>>> segment(2 * t);

    auto add_edge = [&](int start, int end, std::pair<int, int> edge) {
        start += t;
        end += t;
        while (start < end) {
            if (start & 1) segment[start++].push_back(edge);
            if (end & 1) segment[--end].push_back(edge);
            start >>= 1;
            end >>= 1;
        }
    };

    std::map<std::pair<int, int>, int> add_edge_time;
    for (int i = 0; i < (int)querys.size(); i++) {
        std::pair<int, int> edge = std::minmax(querys[i].first, querys[i].second);
        if (add_edge_time.contains(edge)) {
            int j = add_edge_time[edge];
            add_edge_time.erase(edge);
            add_edge(j, i, edge);
        } else {
            add_edge_time.emplace(edge, i);
        }
    }
    for (auto [edge, i]: add_edge_time) {
        add_edge(i, querys.size(), edge);
    }

    UndoUnionFind uf(n);
    auto dfs = [&](auto self, int left, int right, int pos) -> void {
        for (auto [u, v]: segment[pos]) uf.merge(u, v);
        if (left + 1 == right) {
            if (left < std::ssize(querys)) {
                func(left, uf);
            }
        } else {
            int mid = (left + right) / 2;
            self(self, left, mid, 2 * pos);
            self(self, mid, right, 2 * pos + 1);
        }
        for (int i = 0; i < std::ssize(segment[pos]); i++) uf.undo();
    };
    dfs(dfs, 0, t, 1);
}

}  //  namespace nono
