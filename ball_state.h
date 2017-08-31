#ifndef BALL_STATE_H
#define BALL_STATE_H

#include <cassert>

#include "opencv2/opencv.hpp"

// Kalman filter usage based on
// http://opencvexamples.blogspot.com/2014/01/kalman-filter-implementation-tracking.html
class BallState {
public:
  BallState()
      : kf_(/* dimension of state */ 6,
            /* dimension of measurement */ 2),
        measured_(2, 1) {}

  virtual ~BallState() = default;

  void update(cv::Point measured);

  cv::Point2f position() const {
    assert(initialized_);
    return cv::Point2f(estimated_.at<float>(0), estimated_.at<float>(1));
  }
  cv::Point2f velocity() const {
    assert(initialized_);
    return cv::Point2f(estimated_.at<float>(2), estimated_.at<float>(3));
  }
  cv::Point2f acceleration() const {
    assert(initialized_);
    return cv::Point2f(estimated_.at<float>(4), estimated_.at<float>(5));
  }

private:
  void initialize(cv::Point measured);

  cv::KalmanFilter kf_;
  cv::Mat estimated_;
  cv::Mat_<float> measured_;
  bool initialized_ = false;
};

#endif /* BALL_STATE_H */
