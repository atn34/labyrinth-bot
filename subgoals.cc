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

const std::vector<Vec2> &subgoals() {
  static const std::vector<Vec2> *result = []() {
    const Vec2 start{346, 29};
    const Vec2 finish{620, 249};

    std::unordered_map<Vec2, Vec2, Vec2Hasher> parents;

    auto img = get_mask_from_file("path.png");
    bitwise_not(*img, *img);

    Mat path = imread("path.png");

    DoBfs(img.get(), start, [&](Vec2 p, Vec2 parent) {
      parents[p] = parent;
      return p != finish;
    });

    auto *out = new std::vector<Vec2>;

    int iter = 0;
    for (auto p = parents.find(finish);
         p != parents.end() && p->second != start;
         p = parents.find(p->second)) {
      if (iter++ % 20 == 0) {
        out->push_back(p->second);
      }
    }
    out->push_back(start);
    return out;
  }();
  return *result;
}

constexpr float kBallRadius = 10;

void add_obstacles_from_circle(
    Vec2 ball_pos, Circle c, std::vector<ObstacleOrGoal> *obstacles) {
  float theta = (c.p - ball_pos).theta();
  float delta_theta = asin((c.r + kBallRadius) / (c.p - ball_pos).Magnitude());

  ObstacleOrGoal obstacle;
  obstacle.type = kCircle;
  obstacle.dist_to_impact = -1;
  obstacle.theta_enter = WrapAngle(theta - delta_theta);
  obstacle.theta_exit = WrapAngle(theta + delta_theta);
  obstacle.circle = c;

  obstacles->push_back(obstacle);
}

void add_obstacles_from_line_segment(
    Vec2 ball_pos, Vec2 p1, Vec2 p2,
    std::vector<ObstacleOrGoal> *obstacles) {
  float theta1 = (p1 - ball_pos).theta();
  float theta2 = (p2 - ball_pos).theta();
  if (std::abs(theta1 - theta2) < M_PI) {
      if (theta1 > theta2) {
        std::swap(theta1, theta2);
        std::swap(p1, p2);
      }
  } else {
      if (theta1 < 0) {
        std::swap(theta1, theta2);
        std::swap(p1, p2);
      }
  }
  ObstacleOrGoal obstacle;
  obstacle.type = kLineSegment;
  obstacle.dist_to_impact = -1;
  obstacle.theta_enter =
      WrapAngle(theta1 - asin(kBallRadius / (p1 - ball_pos).Magnitude()));
  obstacle.theta_exit =
      WrapAngle(theta2 + asin(kBallRadius / (p2 - ball_pos).Magnitude()));
  obstacle.segment = {p1, p2};

  obstacles->push_back(obstacle);
}

void add_obstacles_from_polygon(
    Vec2 ball_pos, const std::vector<Vec2> &polygon,
    std::vector<ObstacleOrGoal> *obstacles) {
  const Vec2 *last = nullptr;
  const Vec2 *first = nullptr;
  for (const auto &vertex : polygon) {
    if (last != nullptr) {
      add_obstacles_from_line_segment(ball_pos, *last, vertex,
                                      obstacles);
    } else {
      first = &vertex;
    }
    last = &vertex;
  }
  add_obstacles_from_line_segment(ball_pos, *last, *first, obstacles);
}

}  // namespace

Subgoals::Subgoals() {
  current_subgoal_ = subgoals().back();
}

float ObstacleOrGoal::DistToImpact(Vec2 ball_pos, float theta) {
  assert(type != kGoal);
  if (type == kLineSegment) {
    return DistanceToImpact(Circle{ball_pos, kBallRadius},
                                      UnitVec2FromTheta(theta), segment);
  } else if (type == kCircle) {
    return DistanceToImpact(Circle{ball_pos, kBallRadius},
                                      UnitVec2FromTheta(theta), circle);
  }
  assert(false);
}

// Relies on the assumption that no pair of obstacles intersect, and no goals
// intersect any obstacles.
Vec2 Subgoals::next_goal(
    Vec2 ball_pos,
    std::function<void(float, float)> debug_callback) {
  obstacles_.clear();
  goals_.clear();
  angles_of_interest_.clear();
  obstacles_in_line_of_sight_.clear();
  for (const auto &polygon : WallPolygons()) {
    add_obstacles_from_polygon(ball_pos, polygon, &obstacles_);
  }
  for (const auto &hole : HoleCenters()) {
    add_obstacles_from_circle(ball_pos, Circle{hole, 10},
                              &obstacles_);
  }
  {
    int goal_index = 0;
    for (const auto &subgoal : subgoals()) {
      ObstacleOrGoal goal;
      goal.type = kGoal;
      goal.dist_to_impact = (subgoal - ball_pos).Magnitude();
      goal.theta_enter = (subgoal - ball_pos).theta();
      goal.goal = subgoal;
      goal.goal_index = goal_index++;
      goals_.push_back(goal);
    }
  }
  for (auto &obstacle : obstacles_) {
    angles_of_interest_.push_back({{obstacle.theta_enter, true}, &obstacle});
    angles_of_interest_.push_back({{obstacle.theta_exit, false}, &obstacle});
  }
  for (auto &goal : goals_) {
    angles_of_interest_.push_back({{goal.theta_enter, true}, &goal});
  }
  std::sort(angles_of_interest_.begin(), angles_of_interest_.end());

  int best_subgoal_index = subgoals().size();

  {
      float theta = angles_of_interest_.begin()->first.first;
      for (auto& obstacle : obstacles_) {
          float d = obstacle.DistToImpact(ball_pos, theta);
          if (d >= 0) {
              obstacles_in_line_of_sight_.insert(&obstacle);
          }
      }
  }

  auto dist_to_impact_before = obstacles_in_line_of_sight_;

  for (const auto &pair : angles_of_interest_) {
    float theta = pair.first.first;

    auto *goal_or_obstacle = pair.second;
    if (goal_or_obstacle->type == kGoal) {
        auto *goal = goal_or_obstacle;
        float dist_to_first_obstacle = -1;
        for (ObstacleOrGoal* obstacle : obstacles_in_line_of_sight_) {
            float d = obstacle->DistToImpact(ball_pos, theta);
            if (d >= 0) {
                if (dist_to_first_obstacle < 0) {
                    dist_to_first_obstacle = d;
                } else {
                    dist_to_first_obstacle = std::min(dist_to_first_obstacle, d);
                }
            }
        }
        if (dist_to_first_obstacle < 0 || goal->dist_to_impact < dist_to_first_obstacle) {
            if (debug_callback) debug_callback(theta, goal->dist_to_impact);
            if (goal->goal_index < best_subgoal_index) {
                current_subgoal_ = goal->goal;
                best_subgoal_index = goal->goal_index;
            }
        }
    } else {
        auto *obstacle = goal_or_obstacle;
        if (pair.first.second) {
            obstacles_in_line_of_sight_.insert(obstacle);
        } else {
            obstacles_in_line_of_sight_.erase(obstacle);
        }
    }
  }

  return current_subgoal_;
}
