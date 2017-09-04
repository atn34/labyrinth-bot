#ifndef GEOMETRY_H
#define GEOMETRY_H

struct Vec {
    float x;
    float y;
};

struct LineSegment {
    Vec x1;
    Vec x2;
};

struct Circle {
    Vec p;
    float r;
};

/// Returns true if |start| traveling in |direction| will hit |target|.
/// Otherwise returns the minimum distance |start| would have to
/// move along |direction| in order to touch |target|.  Preconditions: |start| is
/// not touching |target|, and |direction| is unit.
float DistanceToImpact(Circle start, Vec direction, Circle target);

#endif /* GEOMETRY_H */
