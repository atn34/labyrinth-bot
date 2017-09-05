#ifndef GEOMETRY_H
#define GEOMETRY_H

struct Vec {
    const float x;
    const float y;

    Vec MakeUnit();
};

inline Vec operator-(const Vec& p1, const Vec& p2) {
    return Vec{p1.x - p2.x, p1.y - p2.y};
}

struct LineSegment {
    const Vec p1;
    const Vec p2;
};

struct Circle {
    const Vec p;
    const float r;
};

/// Returns the minimum distance |start| would have to move along |direction| in
/// order to touch |target|, or a negative value if |start| will not touch
/// |target|.  Preconditions: |start| is not touching |target|, and |direction|
/// is unit.
float DistanceToImpact(Circle start, Vec direction, Circle target);
float DistanceToImpact(Circle start, Vec direction, LineSegment target);

#endif /* GEOMETRY_H */
