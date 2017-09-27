#ifndef NEXT_SUBGOAL_H
#define NEXT_SUBGOAL_H

#include <map>
#include <unordered_set>
#include <vector>

#include "camera_properties.h"
#include "geometry.h"

constexpr float kBallRadius = 10 * (CAMERA_WIDTH / 640.0);
constexpr float kHoleRadius = 10 * (CAMERA_WIDTH / 640.0);

enum ObstacleOrGoalType { kLineSegment, kCircle, kGoal };

struct ObstacleOrGoal {
  enum ObstacleOrGoalType type;
  float dist_to_impact;
  float theta_enter;
  float theta_exit;

  float DistToImpact(Vec2 ball_pos, float theta);

  union {
    struct { /* line segment */
      LineSegment segment;
    };

    struct { /* circle */
      Circle circle;
    };

    struct { /* goal */
      Vec2 goal;
      int goal_index;
    };
  };
};

class Subgoals {
 public:
  Subgoals();
  virtual ~Subgoals() = default;

  Vec2 next_goal(Vec2 ball_pos, bool *touching_obstacle,
                 std::function<void(float, float)> debug_callback = nullptr);

 private:
  Vec2 current_subgoal_;

  std::vector<ObstacleOrGoal> obstacles_;
  std::vector<ObstacleOrGoal> goals_;
  std::vector<std::pair<std::pair<float, bool>, ObstacleOrGoal *>>
      angles_of_interest_;
  std::unordered_set<ObstacleOrGoal *> obstacles_in_line_of_sight_;
};

#endif /* NEXT_SUBGOAL_H */
