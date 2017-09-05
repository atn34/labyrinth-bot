#ifndef NEXT_SUBGOAL_H
#define NEXT_SUBGOAL_H

#include "geometry.h"

#include "opencv2/opencv.hpp"

enum AngleOfInterestType { kLineSegment, kCircle, kGoal };

struct AngleOfInterest {
    float theta;
    enum AngleOfInterestType type;
    float dist_to_impact;
    float theta_enter;
    float theta_exit;

    union {
        struct { /* line segment */
            LineSegment segment;
        };

        struct { /* circle */
            Circle circle;
        };

        struct { /* goal */
            Vec2 p;
        };
    };
};

class Subgoals {
public:
  Subgoals() = default;
  virtual ~Subgoals() = default;

  cv::Point next_goal(cv::Point ball_pos);

private:
  std::vector<AngleOfInterest> angles_of_interest_;
};

#endif /* NEXT_SUBGOAL_H */
