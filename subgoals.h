#ifndef NEXT_SUBGOAL_H
#define NEXT_SUBGOAL_H

#include "opencv2/opencv.hpp"

class Subgoals {
public:
  Subgoals() = default;
  virtual ~Subgoals() = default;

  cv::Point next_goal(cv::Point ball_pos);

private:
  int next_goal_index_ = 0;
};

#endif /* NEXT_SUBGOAL_H */
