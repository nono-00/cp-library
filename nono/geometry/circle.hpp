#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>
#include <vector>

#include "./base.hpp"
#include "./segment.hpp"

namespace nono {

struct Circle {
    Circle(): r(0), c() {}
    Circle(Real r, Point c): r(r), c(c) {}
    Real r;
    Point c;
};

bool intersect(Circle c, Line l) {
    return lte(distance(l, c.c), c.r);
}

bool contain(Circle c, Point p) {
    return lte(distance(c.c, p), c.r);
}

bool intersect(Circle c, Point p) {
    return eq(distance(c.c, p), c.r);
}

//  円の内部（境界は含まない）にpが存在するか
bool strictly_contain(Circle c, Point p) {
    return lt(distance(c.c, p), c.r);
}

//  円周、円内に交点が存在するかどうか
//  円内に線分を内包している場合もtrueと判定される
bool intersect(Circle c, Segment s) {
    return lte(distance(s, c.c), c.r);
}

//  4 -> 交差しない, 内包しない
//  3 -> 外接する
//  2 -> 交差する
//  1 -> 内接する
//  0 -> 内包する
int intersect(Circle lhs, Circle rhs) {
    if (gt(lhs.r, rhs.r)) std::swap(lhs, rhs);
    Real d = distance(lhs.c, rhs.c);
    if (lt(lhs.r + rhs.r, d)) return 4;
    if (eq(lhs.r + rhs.r, d)) return 3;
    if (gt(lhs.r + d, rhs.r)) return 2;
    if (eq(lhs.r + d, rhs.r)) return 1;
    return 0;
}

Points crosspoint(Circle lhs, Circle rhs) {
    assert(intersect(lhs, rhs) != 0);
    assert(intersect(lhs, rhs) != 4);
    Real d = abs(lhs.c - rhs.c);
    if (eq(d, lhs.r + rhs.r)) return {lhs.c + lhs.r * (rhs.c - lhs.c) / d};
    Real a = std::acos((lhs.r * lhs.r + d * d - rhs.r * rhs.r) / (2 * lhs.r * d));
    Real t = arg(rhs.c - lhs.c);
    return {lhs.c + polar(lhs.r, t + a), lhs.c + polar(lhs.r, t - a)};
}

Points crosspoint(Circle c, Line l) {
    assert(intersect(c, l));
    Vector p = projection(l, c.c);
    Real t = std::sqrt(c.r * c.r - norm(c.c - p));
    if (eq(t, 0)) return {p};
    Vector e = (l.p1 - l.p0) / abs(l.p1 - l.p0);
    return {p + t * e, p + -t * e};
}

Points crosspoint(Circle c, Segment s) {
    assert(intersect(c, s));
    auto cps = crosspoint(c, Line(s));
    Points temp;
    for (auto cp: cps) {
        if (intersect(s, cp)) temp.push_back(cp);
    }
    return temp;
}

//  内接円
Circle incircle(Point p0, Point p1, Point p2) {
    assert(std::abs(ccw(p0, p1, p2)) == 1);
    Point c = crosspoint(bisector(p0, p1, p2), bisector(p1, p2, p0));
    //  三角形の面積の2倍 / 周長
    Real double_area = std::abs(cross(p1 - p0, p2 - p0));
    Real perimeter = distance(p0, p1) + distance(p1, p2) + distance(p2, p0);
    return Circle(double_area / perimeter, c);
}

//  外接円
Circle circumscribed_circle(Point p0, Point p1, Point p2) {
    Point c = crosspoint(perpendicular_bisector(Segment(p0, p1)), perpendicular_bisector(Segment(p0, p2)));
    return Circle(distance(p0, c), c);
}

Real area(Circle c) {
    return c.r * c.r * PI;
}

//  点pを通る円cの接点
Points tangent(Circle c, Point p) {
    assert(lte(c.r, distance(c.c, p)));
    return crosspoint(c, Circle(std::sqrt(norm(c.c - p) - c.r * c.r), p));
}

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
