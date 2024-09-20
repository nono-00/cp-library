# nonolib

## 命名規則メモ
- ファイル名 `kebab-case`
- 変数名     `snake_case`
- 関数名     `snake_case`
- 型名       `UpperCamelCase`
- 他にある?  `??????????`

## 他にいろいろ
- 変数名は分かりやすく
- 命名は長くなっても構わない
- `#include <bits/stdc++/h>`はしない
- `using namespace std;`は基本的にしない
  - 使うならスコープに入れる
- `namespace nono`に入れる
- ドキュメントは書かない
- 代わりにコメントをたくさん入れる
- `/// brief : {}`というように書くと概要が書かれる
- public, privateは使う
- 基本的にはclassを使う
- メモリアロケートは全て new で, deallocateはしない
- よくあるmonoidはValue, Actを型として持つ
- めちゃくちゃassertを入れる
- 計算量が悪化する/定数倍がめちゃくちゃ悪くなるassertは`if constexpr (is_debug())`に入れる
