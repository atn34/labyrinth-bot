#ifndef NEXT_SUBGOAL_H
#define NEXT_SUBGOAL_H

#include <map>
#include <unordered_set>
#include <vector>

#include "geometry.h"

enum ObstacleOrGoalType { kLineSegment, kCircle, kGoal };

struct ObstacleOrGoal {
  enum ObstacleOrGoalType type;
  float dist_to_impact;
  float theta_enter;
  float theta_exit;

  float DistToImpact(Vec2 ball_pos, float theta);

  union {
    struct {/* line segment */
      LineSegment segment;
    };

    struct {/* circle */
      Circle circle;
    };

    struct {/* goal */
      Vec2 goal;
      int goal_index;
    };
  };
};

class Subgoals {
 public:
  Subgoals() = default;
  virtual ~Subgoals() = default;

  Vec2 next_goal(Vec2 ball_pos,
                 std::function<void(float, float)>
                     debug_callback = nullptr);

 private:
  std::vector<ObstacleOrGoal> obstacles_and_goals_;
  std::vector<std::pair<std::pair<float, bool>, ObstacleOrGoal *>> angles_of_interest_;
  std::map<float, std::unordered_set<ObstacleOrGoal *>> obstacles_by_dist_to_impact_;
};

#endif /* NEXT_SUBGOAL_H */
