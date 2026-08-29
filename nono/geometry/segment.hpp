#pragma once

#include <cassert>
#include <cmath>

#include "./base.hpp"

namespace nono {

struct Segment {
    Segment(): p0(), p1() {}
    Segment(Point p0, Point p1): p0(p0), p1(p1) {}
    Point p0, p1;
};

struct Line {
    Line(): p0(), p1() {}
    Line(Point p0, Point p1): p0(p0), p1(p1) {}
    explicit Line(Segment s): p0(s.p0), p1(s.p1) {}
    Point p0, p1;
};

Real distance(Line l, Point p) {
    return std::abs(cross(l.p1 - l.p0, p - l.p0)) / abs(l.p1 - l.p0);
}

Real distance(Segment s, Point p) {
    if (lt(dot(s.p1 - s.p0, p - s.p0), 0)) {
        return distance(p, s.p0);
    }
    if (lt(dot(s.p0 - s.p1, p - s.p1), 0)) {
        return distance(p, s.p1);
    }
    return distance(Line(s), p);
}

bool intersect(Segment lhs, Segment rhs) {
    return ccw(lhs.p0, lhs.p1, rhs.p0) * ccw(lhs.p0, lhs.p1, rhs.p1) <= 0
           && ccw(rhs.p0, rhs.p1, lhs.p0) * ccw(rhs.p0, rhs.p1, lhs.p1) <= 0;
}

bool intersect(Segment s, Point p) {
    return ccw(s.p0, s.p1, p) == 0;
}

Real distance(Segment lhs, Segment rhs) {
    if (intersect(lhs, rhs)) {
        return Real(0);
    }
    return std::min({distance(lhs, rhs.p0), distance(lhs, rhs.p1), distance(rhs, lhs.p0), distance(rhs, lhs.p1)});
}

bool parallel(Segment lhs, Segment rhs) {
    return parallel(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

bool parallel(Line lhs, Line rhs) {
    return parallel(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

Point crosspoint(Line lhs, Line rhs) {
    assert(!parallel(lhs, rhs));
    Real a = cross(lhs.p1 - lhs.p0, rhs.p1 - rhs.p0);
    Real b = cross(lhs.p1 - lhs.p0, lhs.p1 - rhs.p0);
    return rhs.p0 + (rhs.p1 - rhs.p0) * b / a;
}

bool intersect(Line l, Segment s) {
    return ccw(l.p0, l.p1, s.p0) * ccw(l.p0, l.p1, s.p1) <= 0;
}

bool orthogonal(Segment lhs, Segment rhs) {
    return orthogonal(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

bool orthogonal(Line lhs, Line rhs) {
    return orthogonal(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

Point crosspoint(Segment lhs, Segment rhs) {
    assert(intersect(lhs, rhs));
    assert(!parallel(lhs, rhs));
    Real d0 = distance(Line(lhs.p0, lhs.p1), rhs.p0);
    Real d1 = distance(Line(lhs.p0, lhs.p1), rhs.p1);
    return rhs.p0 + (rhs.p1 - rhs.p0) * (d0 / (d0 + d1));
}

Point projection(Segment s, Point p) {
    Vector a = p - s.p0;
    Vector b = s.p1 - s.p0;
    Real t = dot(a, b) / norm(b);
    return s.p0 + t * b;
}

Point projection(Line l, Point p) {
    Vector a = p - l.p0;
    Vector b = l.p1 - l.p0;
    Real t = dot(a, b) / norm(b);
    return l.p0 + t * b;
}

Point reflection(Segment s, Point p) {
    return p + (projection(s, p) - p) * Real(2);
}

Point reflection(Line l, Point p) {
    return p + (projection(l, p) - p) * Real(2);
}

//  点pを通る直線lの垂線
Line perpendicular(Line l, Point p) {
    Vector v = l.p1 - l.p0;
    return Line(p, p + Vector(-v.y, v.x));
}

//  垂直二等分線
Line perpendicular_bisector(Segment s) {
    return perpendicular(Line(s), (s.p0 + s.p1) / 2);
}

//  二等分線
//  p0 - p1 - p2の小さいほうの角を二等分する直線
Line bisector(Point p0, Point p1, Point p2) {
    Real a = arg(p0 - p1);
    Real b = arg(p2 - p1);
    if (gt(a, b)) std::swap(a, b);
    Real mid = (a + b) / 2;
    if (gt(b - a, PI)) mid -= PI;
    return Line(p1, p1 + polar(1, mid));
}

}  //  namespace nono
