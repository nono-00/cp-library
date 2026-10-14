#pragma once

//  「貼ったら使える」ための集約ヘッダ。個々のbeats-monoidはnono/structure/beats-monoid/
//  以下に1ファイル1構造体で分割されている。テストや他ヘッダから特定のbeats-monoid
//  だけを使う場合は、verify時の依存範囲を絞るためにこちらではなく個別ファイルを
//  直接includeすることを推奨する。
//
//  beats-monoidはact-monoidとほぼ同じだが、mappingが std::optional<Value> を返す。
//  区間の集約値だけでは作用後の値を決められないときに std::nullopt を返すと、
//  SegmentTreeBeatsが子に降りて計算し直す。1要素の値に対しては必ず値を返すこと

#include "./beats-monoid/range-chmax-range-sum.hpp"
#include "./beats-monoid/range-chmin-boot-range-sum.hpp"
#include "./beats-monoid/range-chmin-chmax-add-range-sum.hpp"
#include "./beats-monoid/range-chmin-chmax-range-sum.hpp"
#include "./beats-monoid/range-chmin-range-sum.hpp"
#include "./beats-monoid/range-divide-update-range-sum.hpp"
