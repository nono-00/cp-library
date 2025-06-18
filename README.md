# nonolib

## 基本原則
貼ったら使える

## 命名規則
- ファイル名 `kebab-case`
- 変数名     `snake_case`
- 関数名     `snake_case`
- namespace  `snake_case`
- 定数名     `UPPER_SNAKE_CASE`
- 型名       `UpperCamelCase`

## 他にいろいろ
- 変数名は分かりやすく
    - 命名は長くなっても構わない
- `#include <bits/stdc++/h>`はしない
- `using namespace std;`は基本的にしない
    - 使うならスコープに入れる
- `namespace nono`に入れる
- 陽にドキュメントは書かない
    - 代わりにファイル内にコメントを書く
- public, privateは使う
- 基本的にはclassを使う
- たくさんassertを入れる
- 計算量が悪化する/定数倍がめちゃくちゃ悪くなるassertは`if constexpr (is_debug())`に入れる
- 添字の型は基本的に`int, long long`
