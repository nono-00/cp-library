#pragma once

#include <cassert>
#include <cmath>

#include "./base.hpp"

namespace nono {

//  # Segment
//  p0とp1を端点とする線分
//  p0 = p1 だと、距離・射影・交点の計算が壊れる
struct Segment {
    Segment(): p0(), p1() {}
    Segment(Point p0, Point p1): p0(p0), p1(p1) {}
    Point p0, p1;
};

//  # Line
//  p0とp1を通る直線
//  p0 = p1 だと、距離・射影・交点の計算が壊れる
struct Line {
    Line(): p0(), p1() {}
    Line(Point p0, Point p1): p0(p0), p1(p1) {}
    explicit Line(Segment s): p0(s.p0), p1(s.p1) {}
    Point p0, p1;
};

//  # distance(l, p)
//  直線lと点pの距離
Real distance(Line l, Point p) {
    return std::abs(cross(l.p1 - l.p0, p - l.p0)) / abs(l.p1 - l.p0);
}

//  # distance(s, p)
//  線分sと点pの距離
Real distance(Segment s, Point p) {
    if (lt(dot(s.p1 - s.p0, p - s.p0), 0)) {
        return distance(p, s.p0);
    }
    if (lt(dot(s.p0 - s.p1, p - s.p1), 0)) {
        return distance(p, s.p1);
    }
    return distance(Line(s), p);
}

//  # intersect(lhs, rhs)
//  線分どうしが共有点を持つか。端点で接する場合も、重なる場合もtrue
bool intersect(Segment lhs, Segment rhs) {
    return ccw(lhs.p0, lhs.p1, rhs.p0) * ccw(lhs.p0, lhs.p1, rhs.p1) <= 0
           && ccw(rhs.p0, rhs.p1, lhs.p0) * ccw(rhs.p0, rhs.p1, lhs.p1) <= 0;
}

//  # intersect(s, p)
//  点pが線分s上にあるか。端点もtrue
bool intersect(Segment s, Point p) {
    return ccw(s.p0, s.p1, p) == 0;
}

//  # distance(lhs, rhs)
//  線分どうしの距離。共有点があれば0
Real distance(Segment lhs, Segment rhs) {
    if (intersect(lhs, rhs)) {
        return Real(0);
    }
    return std::min({distance(lhs, rhs.p0), distance(lhs, rhs.p1), distance(rhs, lhs.p0), distance(rhs, lhs.p1)});
}

//  # parallel(lhs, rhs)
//  線分どうしが平行か
bool parallel(Segment lhs, Segment rhs) {
    return parallel(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

//  # parallel(lhs, rhs)
//  直線どうしが平行か
bool parallel(Line lhs, Line rhs) {
    return parallel(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

//  # crosspoint(lhs, rhs)
//  直線どうしの交点
//  平行だと壊れる
Point crosspoint(Line lhs, Line rhs) {
    assert(!parallel(lhs, rhs));
    Real a = cross(lhs.p1 - lhs.p0, rhs.p1 - rhs.p0);
    Real b = cross(lhs.p1 - lhs.p0, lhs.p1 - rhs.p0);
    return rhs.p0 + (rhs.p1 - rhs.p0) * b / a;
}

//  # intersect(l, s)
//  直線lと線分sが共有点を持つか
//  sがl上にあり、両端点がどちらもl.p0より手前、またはどちらもl.p1より先にあるときは、
//  共有点があるのにfalseを返す
bool intersect(Line l, Segment s) {
    return ccw(l.p0, l.p1, s.p0) * ccw(l.p0, l.p1, s.p1) <= 0;
}

//  # orthogonal(lhs, rhs)
//  線分どうしが直交する向きか。共有点を持つかどうかは見ない
bool orthogonal(Segment lhs, Segment rhs) {
    return orthogonal(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

//  # orthogonal(lhs, rhs)
//  直線どうしが直交するか
bool orthogonal(Line lhs, Line rhs) {
    return orthogonal(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

//  # crosspoint(lhs, rhs)
//  線分どうしの交点
//  共有点がない、または平行だと壊れる
Point crosspoint(Segment lhs, Segment rhs) {
    assert(intersect(lhs, rhs));
    assert(!parallel(lhs, rhs));
    Real d0 = distance(Line(lhs.p0, lhs.p1), rhs.p0);
    Real d1 = distance(Line(lhs.p0, lhs.p1), rhs.p1);
    return rhs.p0 + (rhs.p1 - rhs.p0) * (d0 / (d0 + d1));
}

//  # projection(s, p)
//  点pから、線分sを通る直線へ下ろした垂線の足。線分の外になることもある
Point projection(Segment s, Point p) {
    Vector a = p - s.p0;
    Vector b = s.p1 - s.p0;
    Real t = dot(a, b) / norm(b);
    return s.p0 + t * b;
}

//  # projection(l, p)
//  点pから直線lへ下ろした垂線の足
Point projection(Line l, Point p) {
    Vector a = p - l.p0;
    Vector b = l.p1 - l.p0;
    Real t = dot(a, b) / norm(b);
    return l.p0 + t * b;
}

//  # reflection(s, p)
//  線分sを通る直線に関して、点pと対称な点
Point reflection(Segment s, Point p) {
    return p + (projection(s, p) - p) * Real(2);
}

//  # reflection(l, p)
//  直線lに関して、点pと対称な点
Point reflection(Line l, Point p) {
    return p + (projection(l, p) - p) * Real(2);
}

//  # perpendicular(l, p)
//  点pを通り、直線lに垂直な直線
Line perpendicular(Line l, Point p) {
    Vector v = l.p1 - l.p0;
    return Line(p, p + Vector(-v.y, v.x));
}

//  # perpendicular_bisector(s)
//  線分sの垂直二等分線
Line perpendicular_bisector(Segment s) {
    return perpendicular(Line(s), (s.p0 + s.p1) / 2);
}

//  # bisector(p0, p1, p2)
//  p1を頂点とする角 p0 p1 p2 の二等分線。小さいほうの角を二等分する
Line bisector(Point p0, Point p1, Point p2) {
    Real a = arg(p0 - p1);
    Real b = arg(p2 - p1);
    if (gt(a, b)) std::swap(a, b);
    Real mid = (a + b) / 2;
    if (gt(b - a, PI)) mid -= PI;
    return Line(p1, p1 + polar(1, mid));
}

}  //  namespace nono
