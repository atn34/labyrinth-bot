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
      if (iter++ % 5 == 0) {
        out->push_back(p->second);
      }
    }
    out->push_back(start);

    std::reverse(out->begin(), out->end());
    return out;
  }();
  return *result;
}

constexpr float kBallRadius = 10;

void add_obstacles_from_circle(
    Vec2 ball_pos, Circle c, std::vector<ObstacleOrGoal> *obstacles_and_goals) {
  float theta = (c.p - ball_pos).theta();
  float delta_theta = acos((c.r + kBallRadius) / (c.p - ball_pos).Magnitude());

  ObstacleOrGoal obstacle;
  obstacle.type = kLineSegment;
  obstacle.dist_to_impact = (c.r + kBallRadius) / tan(delta_theta);
  obstacle.theta_enter = theta + delta_theta;
  obstacle.theta_exit = theta - delta_theta;
  obstacle.circle = c;

  obstacles_and_goals->push_back(obstacle);
}

void add_obstacles_from_line_segment(
    Vec2 ball_pos, Vec2 p1, Vec2 p2,
    std::vector<ObstacleOrGoal> *obstacles_and_goals) {
  float theta1 = (p1 - ball_pos).theta();
  float theta2 = (p2 - ball_pos).theta();
  if (theta1 > theta2) {
    using std::swap;
    swap(theta1, theta2);
    swap(p1, p2);
  }
  ObstacleOrGoal obstacle;
  obstacle.type = kLineSegment;
  obstacle.dist_to_impact = (p1 - ball_pos).Magnitude();
  obstacle.theta_enter =
      theta1 - acos(kBallRadius / (p1 - ball_pos).Magnitude());
  obstacle.theta_exit =
      theta2 + acos(kBallRadius / (p2 - ball_pos).Magnitude());
  obstacle.segment = {p1, p2};

  obstacles_and_goals->push_back(obstacle);
}

void add_obstacles_from_polygon(
    Vec2 ball_pos, const std::vector<Vec2> &polygon,
    std::vector<ObstacleOrGoal> *obstacles_and_goals) {
  const Vec2 *last = nullptr;
  const Vec2 *first = nullptr;
  for (const auto &vertex : polygon) {
    if (last != nullptr) {
      add_obstacles_from_line_segment(ball_pos, *last, vertex,
                                      obstacles_and_goals);
    } else {
      first = &vertex;
    }
    last = &vertex;
  }
  add_obstacles_from_line_segment(ball_pos, *last, *first, obstacles_and_goals);
}

}  // namespace

void ObstacleOrGoal::UpdateDistToImpact(Vec2 ball_pos, float theta) {
  assert(type != kGoal);
  if (type == kLineSegment) {
    dist_to_impact = DistanceToImpact(Circle{ball_pos, kBallRadius},
                                      UnitVec2FromTheta(theta), segment);
  } else if (type == kCircle) {
    dist_to_impact = DistanceToImpact(Circle{ball_pos, kBallRadius},
                                      UnitVec2FromTheta(theta), circle);
  }
}

// Relies on the assumption that no pair of obstacles intersect, and no goals
// intersect any obstacles.
Vec2 Subgoals::next_goal(
    Vec2 ball_pos,
    std::function<void(float, const ObstacleOrGoal *)> debug_callback) {
  obstacles_and_goals_.clear();
  angles_of_interest_.clear();
  obstacles_by_dist_to_impact_.clear();
  for (const auto &polygon : WallPolygons()) {
    add_obstacles_from_polygon(ball_pos, polygon, &obstacles_and_goals_);
  }
  for (const auto &hole : HoleCenters()) {
    add_obstacles_from_circle(ball_pos, Circle{hole, 16},
                              &obstacles_and_goals_);
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
      obstacles_and_goals_.push_back(goal);
    }
  }
  for (auto &obstacle_or_goal : obstacles_and_goals_) {
    angles_of_interest_[obstacle_or_goal.theta_enter] = &obstacle_or_goal;
    ;
    if (obstacle_or_goal.type != kGoal) {
      angles_of_interest_[obstacle_or_goal.theta_exit] = &obstacle_or_goal;
      ;
    }
  }

  Vec2 best_subgoal;
  int best_subgoal_index = subgoals().size();
  for (const auto &pair : angles_of_interest_) {
    float theta = pair.first;
    if (pair.second->type == kGoal) {
      const ObstacleOrGoal *goal = pair.second;

      auto *first_obstacle = obstacles_by_dist_to_impact_.size() > 0
                                 ? obstacles_by_dist_to_impact_.begin()->second
                                 : nullptr;
      if (debug_callback != nullptr && first_obstacle != nullptr) {
        debug_callback(theta, first_obstacle);
      }
      if (first_obstacle != nullptr) {
        first_obstacle->UpdateDistToImpact(ball_pos, theta);
        if (first_obstacle->dist_to_impact < goal->dist_to_impact &&
            goal->goal_index < best_subgoal_index) {
          best_subgoal_index = goal->goal_index;
          best_subgoal = goal->goal;
        }
      } else {
        if (goal->goal_index < best_subgoal_index) {
          best_subgoal_index = goal->goal_index;
          best_subgoal = goal->goal;
        }
      }
    } else {
      const ObstacleOrGoal *obstacle = pair.second;
      if (theta == obstacle->theta_exit) {
        obstacles_by_dist_to_impact_.erase(obstacle->dist_to_impact);
      } else {
        // Update dist_to_impact for current closest obstacle if there is one.
        if (obstacles_by_dist_to_impact_.size() > 0) {
          ObstacleOrGoal *closest =
              obstacles_by_dist_to_impact_.begin()->second;
          obstacles_by_dist_to_impact_.erase(
              obstacles_by_dist_to_impact_.begin());
          closest->UpdateDistToImpact(ball_pos, theta);
          obstacles_by_dist_to_impact_[closest->dist_to_impact] = closest;
        }

        ObstacleOrGoal *new_obstacle = pair.second;
        new_obstacle->UpdateDistToImpact(ball_pos, theta);
        obstacles_by_dist_to_impact_[new_obstacle->dist_to_impact] =
            new_obstacle;
      }
    }
  }

  return best_subgoal;
}
