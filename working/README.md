# Working Directory

## Document Comment Memo

### example
``` cpp
//  brief:
//  - グラフ構造体
//
//  tparam:
//  - `T`: 辺の重みの型 //  - `directed`: 有向かどうか
//
//  note:
//  - 有向グラフのaliasとして `DiGraph` が存在する
//  - 以下の条件が満たさないと壊れる
//  \ \ 1. `size()` が頂点数を返す
//  \ \ 2. `operator[]` が `range-based for` に対応している
```

### symbol
- `brief`: 概要
- `details`: 詳細
- `complexity`: 計算量
- `tparam`: template引数
- `param`: 引数
- `pre`: 事前条件
- `return`: 戻り値
- `depend`: 依存するライブラリ
- `note`: メモ、注意点
- `todo`: todo
- `see`: 関連するドキュメント

### format
#### simple
``` cpp
//  {symbol}:
//  - {description}
```

#### with parametor
``` cpp
//  {symbol}:
//  - {param}: {description}
```

#### numbered list
``` cpp
//  {symbol}:
//  \ \ 1. {description}
//  \ \ 2. {description}
```

## Other
- `./make-library-template.sh`: 引数に与えたライブラリディレクトリを`./.template`ディレクトリから作成
- `./compile-all-test-file.sh`: `test`ディレクトリ内のファイルをコンパイルする
- `./.compile_check.sh`: `./compile-all-test-file.sh`内で使用する
