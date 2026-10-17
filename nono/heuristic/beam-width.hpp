#pragma once

#include <algorithm>
#include <cassert>

#include "./timer.hpp"

namespace nono {

//  # BeamWidthController
//  ビームサーチの各手で残す候補数を、残り時間から決める。beam_searchとlayered_beam_searchが使う。
//  各手の展開を終えるたびに、それまでの「経過時間 / 展開したノード数」を1ノードあたりの時間とみなし、
//  残り時間を残り手数で割った時間で展開できるだけの数を次の手に使う。
//  1ノードあたりの時間に全体の平均を使うのは、一時的な遅れ(OSによる中断など)で幅が急に縮むのを避けるため。
//  手が進むとノードあたりの時間が変わる問題では見積もりが遅れるが、ずれた分は残り時間に表れ、以降の手で取り戻す
class BeamWidthController {
  public:
    //  # BeamWidthController(max_width, time_limit, expected_turn, max_turn)
    //  time_limitはミリ秒で、作った時点から測る。0以下なら、next()は常にmax_widthを返す。
    //  残り手数はexpected_turn手目まで(0以下ならmax_turn)と見込み、それを超えたらmax_turn手目までとみなす。
    //  max_width > 0 でないと壊れる
    BeamWidthController(int max_width, double time_limit, int expected_turn, int max_turn)
        : max_width_(max_width),
          time_limit_(time_limit),
          expected_turn_(expected_turn > 0 ? std::min(expected_turn, max_turn) : max_turn),
          max_turn_(max_turn),
          timer_(time_limit) {
        assert(max_width > 0);
    }

    //  # next(turn, expanded)
    //  turn手目でexpanded個のノードを展開した後に呼ぶ。turn + 1手目に残す候補数を返す。
    //  返す値はmax_widthを超えず、1を下回らない。時間を使い切った後は1を返す
    int next(int turn, int expanded) {
        if (time_limit_ <= 0) return max_width_;
        expanded_ += expanded;
        double elapsed = timer_.elapsed();
        double remaining = time_limit_ - elapsed;
        if (remaining <= 0) return 1;
        if (elapsed <= 0 || expanded_ == 0) return max_width_;
        int horizon = turn + 1 < expected_turn_ ? expected_turn_ : max_turn_;
        int remaining_turns = std::max(horizon - (turn + 1), 1);
        double width = remaining / remaining_turns / (elapsed / static_cast<double>(expanded_));
        return static_cast<int>(std::clamp(width, 1.0, static_cast<double>(max_width_)));
    }

  private:
    int max_width_;
    double time_limit_;
    int expected_turn_;
    int max_turn_;
    Timer timer_;
    long long expanded_ = 0;
};

}  //  namespace nono
