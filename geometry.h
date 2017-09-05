#ifndef GEOMETRY_H
#define GEOMETRY_H

struct Vec2 {
    float x;
    float y;

    Vec2 MakeUnit();
    float theta() const;
    float MagnitudeSquared() const;
    float Magnitude() const;
};

inline Vec2 operator-(const Vec2& p1, const Vec2& p2) {
    return Vec2{p1.x - p2.x, p1.y - p2.y};
}

struct LineSegment {
    Vec2 p1;
    Vec2 p2;
};

struct Circle {
    Vec2 p;
    float r;
};

/// Returns the minimum distance |start| would have to move along |direction| in
/// order to touch |target|, or a negative value if |start| will not touch
/// |target|.  Preconditions: |start| is not touching |target|, and |direction|
/// is unit.
float DistanceToImpact(Circle start, Vec2 direction, Circle target);
float DistanceToImpact(Circle start, Vec2 direction, LineSegment target);

#endif /* GEOMETRY_H */
