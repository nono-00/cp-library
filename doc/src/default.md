# nonolib
## algorithm
## ds
### [binary-trie](./ds/binary-trie.md)
非負整数用のset. kth_elementやxormin/max, 全部にxorなどができる
### [csr-array](./ds/csr-array.md)
Compressed Sparse Row Array. 二次元配列を一次元に潰すやつ. 使ったことがない.
### [disjoint-sparse-table](./ds/disjoint-sparse-table.md)
静的な区間積クエリを前処理\\( O(n \log n) \\), クエリ \\( O(1) \\)で. Disjointなので, 冪等律を要求しない.
### [dual-segment-tree](./ds/dual-segment-tree.md)
区間作用, 一点取得のsegment tree. 作用が可換のもののみに対応.
### [dynamic-li-chao-tree](./ds/dynamic-li-chao-tree.md)
直線, 線分追加, ある地点での最小値/最大値取得ができる
### [dynamic-segment-tree-2d](./ds/dynamic-segment-tree-2d.md)
座圧せずに使える二次元segment tree. とっっても遅い
### [dynamic-segment-tree](./ds/dynamic-segment-tree.md)
座圧せずに使えるsegment tree. とても遅い.

**TODO**: 二分探索
### [fenwick-range-tree](./ds/fenwick-range-tree.md)
領域木fenwick tree盛り. 一点加算座標が事前にわかる場合に使える二次元 fenwick tree.
### [fenwick-tree-2d](./ds/fenwick-tree-2d.md)
一点加算、総和取得が \\( O(\log ^ 2 N) \\) で可能
### [fenwick-tree](./ds/fenwick-tree.md)
一点加算区間取得. segment treeよりも定数倍が良い.

**TODO**: 二分探索
### [lazy-segment-tree](./ds/lazy-segment-tree.md)
区間作用 区間取得のsegment tree. acl のインターフェースを少し変えただけ. <https://atcoder.github.io/ac-library/master/document_ja/lazysegtree.html>
### [li-chao-tree](./ds/li-chao-tree.md)

**TODO**: implementation
### [offline-dynamic-connectivity](./ds/offline-dynamic-connectivity.md)
オフライン辺追加/削除クエリを捌きながら, 連結成分を管理するやつ

**TODO**: interfaceがカスかも？
### [potentialized-union-find](./ds/potentialized-union-find.md)
UnionFind with Potential. 可換/非可換どちらでも動く.
### [range-add-fenwick-tree](./ds/range-add-fenwick-tree.md)
区間加算区間取得 Fenwick Tree. lazy segment tree よりも定数倍が良い(はず).
### [range-segment-tree](./ds/range-segment-tree.md)
領域木segment tree盛り. 一点加算座標が事前にわかる場合に使える二次元 segment tree.
### [segment-tree-2d](./ds/segment-tree-2d.md)
一点更新矩形取得 二次元segment tree.
### [segment-tree-beats](./ds/segment-tree-beats.md)

**TODO**: implementation
### [segment-tree](./ds/segment-tree.md)
一点更新 区間取得のsegment tree. <https://atcoder.github.io/ac-library/master/document_ja/segtree.html>
### [sparse-table-2d](./ds/sparse-table-2d.md)
静的な矩形領域積クエリを前処理\\( O(n \log ^ 2 n) \\), クエリ \\( O(1) \\)で. メモリ使用量がバカ.
### [sparse-table](./ds/sparse-table.md)
静的な区間積クエリを前処理\\( O(n \log n) \\), クエリ \\( O(1) \\)で. だいたいDisjointを使った方が良い
### [splay-tree](./ds/splay-tree.md)
deprecated
### [treap](./ds/treap.md)

**TODO**: implementation
### [trie-sequence](./ds/trie-sequence.md)

**TODO**: implementation
### [two-stack-deque](./ds/two-stack-deque.md)
二つのStackをくっつけてDequeにするやつ. 要素の総積を計算できる
### [two-stack-queue](./ds/two-stack-queue.md)
二つのStackをくっつけてQueueにするやつ. 要素の総積を計算できる
### [undo-unionfind](./ds/undo-unionfind.md)
Undoが可能なUnionFind
### [union-find](./ds/union-find.md)
連結成分を管理する
## geometry
### [base](./geometry/base.md)

**TODO**: split
## graph
### [articulation-points](./graph/articulation-points.md)
関節点列挙
### [base](./graph/base.md)
辺/グラフ構造体などの基礎となるデータ構造など.
### [bellman-ford](./graph/bellman-ford.md)
負辺ありの単一始点最短経路問題を解く. \\( O(|E||V|) \\)
### [bfs](./graph/bfs.md)
幅優先探索. 使ったことがない.
### [bridges](./graph/bridges.md)
橋列挙
### [contract](./graph/contract.md)
グラフを縮約する. 二辺連結成分分解や強連結成分分解したあとに使うと便利.
### [cycle-detection](./graph/cycle-detection.md)
サイクル検出. 無向, 有向, 単純, 非単純問わない.
### [dijkstra](./graph/dijkstra.md)
負辺なしの単一始点最短経路問題を解く. \\( O(|E| \log |E|) \\)

**TODO**: 負辺なしかどうかassertion
### [kruskal](./graph/kruskal.md)
最小全域木を求める.
### [segment-edge](./graph/segment-edge.md)
区間に辺を貼るやつ.
### [strongly-connected-components](./graph/strongly-connected-components.md)
強連結成分分解する. 単純非単純を問わない.
### [topological-sort](./graph/topological-sort.md)
トポソする.

**TODO**: DAGかどうかassertion
### [two-edge-connected-components](./graph/two-edge-connected-components.md)
二辺連結成分分解する. 連結非連結, 単純非単純を問わない.
## heuristic
### [index-set](./heuristic/index-set.md)

**TODO**: implementation
### [timer](./heuristic/timer.md)
### [xorshift](./heuristic/xorshift.md)
## math
### [bitwise-and-convolution](./math/bitwise-and-convolution.md)
### [bitwise-or-convolution](./math/bitwise-or-convolution.md)
### [matrix](./math/matrix.md)
### [modint](./math/modint.md)
aclベースのmodint. cin, coutによる入出力に対応.
### [zeta-mobius-transform](./math/zeta-mobius-transform.md)
## string
### [lcp-array](./string/lcp-array.md)
### [manacher](./string/manacher.md)
### [suffix-array](./string/suffix-array.md)
### [z-algorithm](./string/z-algorithm.md)
## structure
### [act-monoid](./structure/act-monoid.md)
ActMonoid全部盛り.
### [beats-monoid](./structure/beats-monoid.md)
### [group](./structure/group.md)
### [monoid-template](./structure/monoid-template.md)
### [monoid](./structure/monoid.md)
Monoid全部盛り.
### [test](./structure/test.md)
結合律などの確認
## tree
### [centroids](./tree/centroids.md)
### [diameter](./tree/diameter.md)
### [heavy-light-decomposition](./tree/heavy-light-decomposition.md)
### [is-tree](./tree/is-tree.md)
木かどうか判定する
### [tree-compressor](./tree/tree-compressor.md)
## utility
### [compressor](./utility/compressor.md)
### [debug](./utility/debug.md)
### [inversion_number](./utility/inversion_number.md)
### [run-length-encoding](./utility/run-length-encoding.md)
