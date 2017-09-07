#ifndef BALL_STATE_H
#define BALL_STATE_H

#include <cassert>

#include "opencv2/opencv.hpp"

#include "geometry.h"

// Kalman filter usage based on
// http://opencvexamples.blogspot.com/2014/01/kalman-filter-implementation-tracking.html
class BallState {
 public:
  BallState()
      : kf_(/* dimension of state */ 6,
            /* dimension of measurement */ 2),
        measured_(2, 1) {}

  virtual ~BallState() = default;

  void update(Vec2 measured);

  void reset();

  Vec2 position() const {
    assert(initialized_);
    return Vec2{estimated_.at<float>(0), estimated_.at<float>(1)};
  }
  Vec2 velocity() const {
    assert(initialized_);
    return Vec2{estimated_.at<float>(2), estimated_.at<float>(3)};
  }
  Vec2 acceleration() const {
    assert(initialized_);
    return Vec2{estimated_.at<float>(4), estimated_.at<float>(5)};
  }

 private:
  void initialize(Vec2 measured);

  cv::KalmanFilter kf_;
  cv::Mat estimated_;
  cv::Mat_<float> measured_;
  bool initialized_ = false;
};

#endif /* BALL_STATE_H */
