#include "geometry.h"

#include <algorithm>
#include <math.h>

namespace {

// Solves ax^2 + bx + c = 0. Returns all real roots.
std::vector<float> solve_quadratic(float a, float b, float c) {
    std::vector<float> result;
    float discriminant =  b * b - 4 * a * c;
    if (discriminant < 0) {
        return result;
    }
    float sqrt_discriminant = sqrt(discriminant);
    result.push_back((-b - sqrt_discriminant) / (2 * a));
    result.push_back((-b + sqrt_discriminant) / (2 * a));
    return result;
}

// Returns the smallest positive element of roots, or a negative number if all are negative.
float dist_from_roots(std::vector<float> roots) {
    float result = -1;
    for (const auto& root : roots) {
        if (root > 0) {
            if (result < 0) {
                result = root;
            } else {
                result = std::min(result, root);
            }
        }
    }
    return result;
}

float DotProduct(Vec2 p1, Vec2 p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

}  // namespace

Vec2 Vec2::MakeUnit() {
    float magnitude = Magnitude();
    return Vec2{x / magnitude, y / magnitude};
}

float Vec2::theta() const {
    return atan2(y, x);
}

float Vec2::MagnitudeSquared() const {
    return x * x + y * y;
}

float Vec2::Magnitude() const {
    return sqrt(MagnitudeSquared());
}

// || start + t * direction - target || = r1 + r2, and then solve for t
float DistanceToImpact(Circle start, Vec2 direction, Circle target) {
    float x = start.p.x - target.p.x;
    float y = start.p.y - target.p.y;
    float x_dir = direction.x;
    float y_dir = direction.y;
    float r_sum = start.r + target.r;

    float a = x_dir * x_dir + y_dir * y_dir;
    float b = 2 * x * x_dir + 2 * y * y_dir;
    float c = x * x + y * y - r_sum * r_sum;

    return dist_from_roots(solve_quadratic(a, b, c));
}

// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line.
// Set formula for distance from line containing (x1, y1) and (x2, y2)
// to (x(t), y(t)) equal to radius of circle and solve for t.
float DistanceToImpact(Circle start, Vec2 direction, LineSegment target) {
    float x1 = target.p1.x;
    float x2 = target.p2.x;
    float y1 = target.p1.y;
    float y2 = target.p2.y;
    float x0 = start.p.x;
    float y0 = start.p.y;
    float x_dir = direction.x;
    float y_dir = direction.y;

    float kNumeratorConstants = y2 * x0 - y1 * x0 - x2 * y0 + x1 * y0 + x2 *
        y1 - y2 * x1;
    float kNumeratorLinearTerms = y2 * x_dir - y1 * x_dir - x2 * y_dir + x1 * y_dir;
    float kDenom = (y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1);

    float a = kNumeratorLinearTerms * kNumeratorLinearTerms;
    float b = 2 * kNumeratorConstants * kNumeratorLinearTerms;
    float c = kNumeratorConstants * kNumeratorConstants - kDenom * start.r * start.r;

    float t = dist_from_roots(solve_quadratic(a, b, c));

    // We want to know if circle at start + t * direction touches target.
    // Project start + t * direction onto target:
    // https://en.wikipedia.org/wiki/Vector_projection.
    Vec2 u = (target.p2 - target.p1).MakeUnit();
    float projected = DotProduct(u, Vec2{x0 + x_dir * t, y0 + y_dir * t} - target.p1);
    float dist_to_segment = -1;
    if (0 <= projected && projected <= (target.p2 - target.p1).Magnitude()) {
        dist_to_segment = t;
    }

    // Also check endpoints. Treat as time to impact for radius 0 circles.
    return dist_from_roots({dist_to_segment, DistanceToImpact(start, direction,
                Circle{target.p1, 0}), DistanceToImpact(start, direction,
                    Circle{target.p2, 0})});
}
