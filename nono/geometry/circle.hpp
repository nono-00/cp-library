#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>
#include <vector>

#include "./base.hpp"
#include "./segment.hpp"

namespace nono {

//  # Circle
//  中心がc、半径がrの円。コンストラクタの引数は (r, c) の順
struct Circle {
    Circle(): r(0), c() {}
    Circle(Real r, Point c): r(r), c(c) {}
    Real r;
    Point c;
};

//  # intersect(c, l)
//  円cと直線lが共有点を持つか。接する場合もtrue
bool intersect(Circle c, Line l) {
    return lte(distance(l, c.c), c.r);
}

//  # contain(c, p)
//  点pが円cの内部または周上にあるか
bool contain(Circle c, Point p) {
    return lte(distance(c.c, p), c.r);
}

//  # intersect(c, p)
//  点pが円cの周上にあるか
bool intersect(Circle c, Point p) {
    return eq(distance(c.c, p), c.r);
}

//  # strictly_contain(c, p)
//  点pが円cの内部にあるか。周上はfalse
bool strictly_contain(Circle c, Point p) {
    return lt(distance(c.c, p), c.r);
}

//  # intersect(c, s)
//  線分sが円cの内部または周と共有点を持つか
//  線分が円の内部に収まっていて周と交わらない場合もtrue
bool intersect(Circle c, Segment s) {
    return lte(distance(s, c.c), c.r);
}

//  # intersect(lhs, rhs)
//  2円の位置関係を、共通接線の本数で返す
//  4 : 離れている
//  3 : 外接する
//  2 : 2点で交わる
//  1 : 内接する。同一の円も1になる
//  0 : 一方が他方の内部にあり、接していない
int intersect(Circle lhs, Circle rhs) {
    if (gt(lhs.r, rhs.r)) std::swap(lhs, rhs);
    Real d = distance(lhs.c, rhs.c);
    if (lt(lhs.r + rhs.r, d)) return 4;
    if (eq(lhs.r + rhs.r, d)) return 3;
    if (gt(lhs.r + d, rhs.r)) return 2;
    if (eq(lhs.r + d, rhs.r)) return 1;
    return 0;
}

//  # crosspoint(lhs, rhs)
//  2円の交点。外接するときは1点、それ以外は2点を返す。内接するときは同じ点が2つ入る
//  intersect(lhs, rhs) が 0 または 4 だと壊れる。中心が一致していても壊れる
Points crosspoint(Circle lhs, Circle rhs) {
    assert(intersect(lhs, rhs) != 0);
    assert(intersect(lhs, rhs) != 4);
    Real d = abs(lhs.c - rhs.c);
    if (eq(d, lhs.r + rhs.r)) return {lhs.c + lhs.r * (rhs.c - lhs.c) / d};
    Real a = std::acos((lhs.r * lhs.r + d * d - rhs.r * rhs.r) / (2 * lhs.r * d));
    Real t = arg(rhs.c - lhs.c);
    return {lhs.c + polar(lhs.r, t + a), lhs.c + polar(lhs.r, t - a)};
}

//  # crosspoint(c, l)
//  円cと直線lの交点。接するときは1点、交わるときは2点を返す
//  共有点がないと壊れる
Points crosspoint(Circle c, Line l) {
    assert(intersect(c, l));
    Vector p = projection(l, c.c);
    Real t = std::sqrt(c.r * c.r - norm(c.c - p));
    if (eq(t, 0)) return {p};
    Vector e = (l.p1 - l.p0) / abs(l.p1 - l.p0);
    return {p + t * e, p + -t * e};
}

//  # crosspoint(c, s)
//  円cの周と線分sの交点。0点から2点を返す
//  線分が円の内部に収まっているときは空になる
//  intersect(c, s) がfalseだと壊れる
Points crosspoint(Circle c, Segment s) {
    assert(intersect(c, s));
    auto cps = crosspoint(c, Line(s));
    Points temp;
    for (auto cp: cps) {
        if (intersect(s, cp)) temp.push_back(cp);
    }
    return temp;
}

//  # incircle(p0, p1, p2)
//  三角形 p0 p1 p2 の内接円
//  3点が同一直線上にあると壊れる
Circle incircle(Point p0, Point p1, Point p2) {
    assert(std::abs(ccw(p0, p1, p2)) == 1);
    Point c = crosspoint(bisector(p0, p1, p2), bisector(p1, p2, p0));
    //  三角形の面積の2倍 / 周長
    Real double_area = std::abs(cross(p1 - p0, p2 - p0));
    Real perimeter = distance(p0, p1) + distance(p1, p2) + distance(p2, p0);
    return Circle(double_area / perimeter, c);
}

//  # circumscribed_circle(p0, p1, p2)
//  三角形 p0 p1 p2 の外接円
//  3点が同一直線上にあると壊れる
Circle circumscribed_circle(Point p0, Point p1, Point p2) {
    Point c = crosspoint(perpendicular_bisector(Segment(p0, p1)), perpendicular_bisector(Segment(p0, p2)));
    return Circle(distance(p0, c), c);
}

//  # area(c)
//  円の面積
Real area(Circle c) {
    return c.r * c.r * PI;
}

//  # tangent(c, p)
//  点pから円cへ引いた接線の接点。pが周上なら1点(p自身)、外部なら2点を返す
//  pが円の内部にあると壊れる
Points tangent(Circle c, Point p) {
    assert(lte(c.r, distance(c.c, p)));
    return crosspoint(c, Circle(std::sqrt(norm(c.c - p) - c.r * c.r), p));
}

//  # tangent(c1, c2)
//  2円の共通接線。本数は intersect(c1, c2) の値と同じ。中心が一致するときは空を返す
//  各直線のp0は半径が大きいほうの円(同じならc1)との接点、p1はもう一方の円との接点
//  2円が接しているとき、その接点を通る接線だけは、p0が接点でp1は接線上の別の点になる
std::vector<Line> tangent(Circle c1, Circle c2) {
    std::vector<Line> ret;
    if (c1.r < c2.r) std::swap(c1, c2);
    Real g = norm(c1.c - c2.c);
    if (eq(g, 0)) return ret;
    Point u = (c2.c - c1.c) / std::sqrt(g);
    Point v = rotate(u, PI * 0.5);
    for (int s: {-1, 1}) {
        Real h = (c1.r + s * c2.r) / std::sqrt(g);
        if (eq(1 - h * h, 0)) {
            ret.emplace_back(c1.c + u * c1.r, c1.c + (u + v) * c1.r);
        } else if (1 - h * h > 0) {
            Point uu = u * h, vv = v * std::sqrt(1 - h * h);
            ret.emplace_back(c1.c + (uu + vv) * c1.r, c2.c - (uu + vv) * c2.r * s);
            ret.emplace_back(c1.c + (uu - vv) * c1.r, c2.c - (uu - vv) * c2.r * s);
        }
    }
    return ret;
}

}  //  namespace nono
