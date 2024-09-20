# nonolib
## algorithm
## ds
### [binary-trie](./binary-trie.md)
非負整数用のset. kth_elementやxormin/max, 全部にxorなどができる
### [csr-array](./csr-array.md)
Compressed Sparse Row Array. 二次元配列を一次元に潰すやつ. 使ったことがない.
### [disjoint-sparse-table](./disjoint-sparse-table.md)
静的な区間積クエリを前処理\\( O(n \log n) \\), クエリ \\( O(1) \\)で. Disjointなので, 冪等律を要求しない.
### [dual-segment-tree](./dual-segment-tree.md)
区間作用, 一点取得のsegment tree. 作用が可換のもののみに対応.
### [dynamic-segment-tree-2d](./dynamic-segment-tree-2d.md)
座圧せずに使える二次元segment tree. とっっても遅い
### [dynamic-segment-tree](./dynamic-segment-tree.md)
座圧せずに使えるsegment tree. とても遅い.

**TODO**: 二分探索
### [fenwick-range-tree](./fenwick-range-tree.md)
領域木fenwick tree盛り. 一点加算座標が事前にわかる場合に使える二次元 fenwick tree.
### [fenwick-tree-2d](./fenwick-tree-2d.md)
一点加算、総和取得が \\( O(\log ^ 2 N) \\) で可能
### [fenwick-tree](./fenwick-tree.md)
一点加算区間取得. segment treeよりも定数倍が良い.

**TODO**: 二分探索
### [lazy-segment-tree](./lazy-segment-tree.md)
区間作用 区間取得のsegment tree. acl のインターフェースを少し変えただけ. <https://atcoder.github.io/ac-library/master/document_ja/lazysegtree.html>
### [offline-dynamic-connectivity](./offline-dynamic-connectivity.md)
オフライン辺追加/削除クエリを捌きながら, 連結成分を管理するやつ

**TODO**: interfaceがカスかも？
### [potentialized-union-find](./potentialized-union-find.md)
UnionFind with Potential. 可換/非可換どちらでも動く.
### [range-add-fenwick-tree](./range-add-fenwick-tree.md)
区間加算区間取得 Fenwick Tree. lazy segment tree よりも定数倍が良い(はず).
### [range-segment-tree](./range-segment-tree.md)
領域木segment tree盛り. 一点加算座標が事前にわかる場合に使える二次元 segment tree.
### [segment-tree-2d](./segment-tree-2d.md)
一点更新矩形取得 二次元segment tree.
### [segment-tree](./segment-tree.md)
一点更新 区間取得のsegment tree. <https://atcoder.github.io/ac-library/master/document_ja/segtree.html>
### [sparse-table-2d](./sparse-table-2d.md)
静的な矩形領域積クエリを前処理\\( O(n \log ^ 2 n) \\), クエリ \\( O(1) \\)で. メモリ使用量がバカ.
### [sparse-table](./sparse-table.md)
静的な区間積クエリを前処理\\( O(n \log n) \\), クエリ \\( O(1) \\)で. だいたいDisjointを使った方が良い
### [splay-tree](./splay-tree.md)
deprecated
### [two-stack-deque](./two-stack-deque.md)
### [two-stack-queue](./two-stack-queue.md)
### [undo-unionfind](./undo-unionfind.md)
### [union-find](./union-find.md)
## geometry
## graph
### [articulation-points](./articulation-points.md)
### [base](./base.md)
### [bellman-ford](./bellman-ford.md)
### [bfs](./bfs.md)
### [bridges](./bridges.md)
### [contract](./contract.md)
### [cycle-detection](./cycle-detection.md)
### [dijkstra](./dijkstra.md)
### [kruskal](./kruskal.md)
### [segment-edge](./segment-edge.md)
### [strongly-connected-components](./strongly-connected-components.md)
### [topological-sort](./topological-sort.md)
### [two-edge-connected-components](./two-edge-connected-components.md)
## heuristic
### [index-set](./index-set.md)
### [timer](./timer.md)
### [xorshift](./xorshift.md)
## math
### [bitwise-and-convolution](./bitwise-and-convolution.md)
### [bitwise-or-convolution](./bitwise-or-convolution.md)
### [matrix](./matrix.md)
### [modint](./modint.md)
### [zeta-mobius-transform](./zeta-mobius-transform.md)
## string
### [](./.md)
### [lcp-array](./lcp-array.md)
### [manacher](./manacher.md)
### [suffix-array](./suffix-array.md)
### [z-algorithm](./z-algorithm.md)
## tree
### [centroids](./centroids.md)
### [diameter](./diameter.md)
### [heavy-light-decomposition](./heavy-light-decomposition.md)
### [is-tree](./is-tree.md)
木かどうか判定する
### [tree-compressor](./tree-compressor.md)
## utility
### [compressor](./compressor.md)
### [debug](./debug.md)
### [inversion_number](./inversion_number.md)
### [run-length-encoding](./run-length-encoding.md)
