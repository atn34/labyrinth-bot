#include "geometry.h"

#include <algorithm>
#include <math.h>

namespace {

// Solves ax^2 + bx + c = 0. Writes real roots to *x1 and *x2, and returns the
// number of real roots.
int solve_quadratic(float a, float b, float c, float* x1, float* x2) {
    float discriminant =  b * b - 4 * a * c;
    if (discriminant < 0) {
        return 0;
    }
    if (discriminant == 0) {
        *x1 = -b / (2 * a);
        return 1;
    }
    float sqrt_discriminant = sqrt(discriminant);
    *x1 = (-b - sqrt_discriminant) / (2 * a);
    *x2 = (-b + sqrt_discriminant) / (2 * a);
    return 2;
}

}  // namespace

// || start + t * direction - target || = r1 + r2, and then solve for t
float DistanceToImpact(Circle start, Vec direction, Circle target) {
    float x = start.p.x - target.p.x;
    float y = start.p.y - target.p.y;
    float x_dir = direction.x;
    float y_dir = direction.y;
    float r_sum = start.r + target.r;

    float a = x_dir * x_dir + y_dir * y_dir;
    float b = 2 * x * x_dir + 2 * y * y_dir;
    float c = x * x + y * y - r_sum * r_sum;

    float root_1;
    float root_2;

    int num_roots = solve_quadratic(a, b, c, &root_1, &root_2);
    if (num_roots == 0) {
        return -1;
    }
    if (num_roots == 1) {
        return root_1;
    }
    if (root_1 >= 0 && root_2 >= 0) {
        return std::min(root_1, root_2);
    }
    if (root_1 >= 0) {
        return root_1;
    }
    if (root_2 >= 0) {
        return root_2;
    }
    return -1;
}

