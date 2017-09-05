#include "subgoals.h"

#include <algorithm>
#include <math.h>
#include <unordered_map>

#include "bfs.h"
#include "geometry.h"
#include "mask_util.h"
#include "walls_and_holes.h"

using namespace cv;

namespace {

struct PointHasher {
    std::size_t operator()(const Point& p) const {
        return p.x ^ (p.y << 1);
    }
};

/* const std::vector<Point> &subgoals() { */
/*   static const std::vector<Point> &result = []() { */
/*     const Point start(346, 29); */
/*     const Point finish(620, 249); */

/*     std::unordered_map<Point, Point, PointHasher> parents; */

/*     auto img = get_mask_from_file("path.png"); */
/*     bitwise_not(*img, *img); */

/*     Mat path = imread("path.png"); */

/*     DoBfs(img.get(), start, [&](Point p, Point parent) { */
/*       parents[p] = parent; */
/*       return p != finish; */
/*     }); */

/*     auto *out = new std::vector<Point>; */

/*     int iter = 0; */
/*     for (auto p = parents.find(finish); */
/*          p != parents.end() && p->second != start; */
/*          p = parents.find(p->second)) { */
/*       if (iter++ % 5 == 0) { */
/*         out->push_back(p->second); */
/*       } */
/*     } */
/*     out->push_back(start); */

/*     std::reverse(out->begin(), out->end()); */
/*     return *out; */
/*   }(); */
/*   return result; */
/* } */

constexpr float kBallRadius = 10;

void add_angles_of_interest_from_circle(Vec2 ball_pos, Circle c,
        std::vector<AngleOfInterest>* angles) {
    float theta = (c.p - ball_pos).theta();
    float delta_theta = acos((c.r + kBallRadius) / (c.p -
                ball_pos).Magnitude());

    AngleOfInterest angle1;
    angle1.theta = theta - delta_theta;
    angle1.type = kLineSegment;
    angle1.dist_to_impact = (c.r + kBallRadius) / tan(delta_theta);
    angle1.circle = c;

    AngleOfInterest angle2;
    angle2.theta = theta + delta_theta;
    angle2.type = kLineSegment;
    angle2.dist_to_impact = (c.r + kBallRadius) / tan(delta_theta);
    angle2.circle = c;

    angle1.theta_enter = angle1.theta;
    angle1.theta_exit = angle2.theta;
    angle2.theta_enter = angle1.theta;
    angle2.theta_exit = angle2.theta;

    angles->push_back(angle1);
    angles->push_back(angle2);
}

void add_angle_of_interest_from_line_segment(Vec2 ball_pos, Vec2 p1,
        Vec2 p2, std::vector<AngleOfInterest>* angles) {
    float theta1 = (p1 - ball_pos).theta();
    float theta2 = (p2 - ball_pos).theta();
    if (theta1 > theta2) {
        using std::swap;
        swap(theta1, theta2);
        swap(p1, p2);
    }
    AngleOfInterest angle1;
    angle1.theta = theta1 - acos(kBallRadius / (p1 - ball_pos).Magnitude());
    angle1.type = kLineSegment;
    angle1.dist_to_impact = (p1 - ball_pos).Magnitude();
    angle1.segment = {p1, p2};

    AngleOfInterest angle2;
    angle2.theta = theta2 + acos(kBallRadius / (p2 - ball_pos).Magnitude());
    angle2.type = kLineSegment;
    angle2.dist_to_impact = (p2 - ball_pos).Magnitude();
    angle2.segment = {p1, p2};

    angle1.theta_enter = angle1.theta;
    angle1.theta_exit = angle2.theta;
    angle2.theta_enter = angle1.theta;
    angle2.theta_exit = angle2.theta;

    angles->push_back(angle1);
    angles->push_back(angle2);
}

void add_angles_of_interest_from_polygon(Vec2 ball_pos, const
        std::vector<Vec2>& polygon, std::vector<AngleOfInterest>* angles) {
    const Vec2 *last = nullptr;
    const Vec2 *first = nullptr;
    for (const auto &vertex : polygon) {
        if (last != nullptr) {
            add_angle_of_interest_from_line_segment(ball_pos, *last, vertex, angles);
        } else {
            first = &vertex;
        }
        last = &vertex;
        add_angle_of_interest_from_line_segment(ball_pos, *last, *first, angles);
    }
}

}  // namespace

Vec2 Subgoals::next_goal(Vec2 ball_pos) {
    angles_of_interest_.clear();
    for (const auto& polygon : WallPolygons()) {
        add_angles_of_interest_from_polygon(ball_pos, polygon, &angles_of_interest_);
    }
    for (const auto& hole : HoleCenters()) {
        add_angles_of_interest_from_circle(ball_pos, Circle{hole, 16}, &angles_of_interest_);
    }
    return Vec2{640 / 3, 30};
}
