#include "ball_state.h"

using namespace cv;

void BallState::update(Point measured) {
  measured_(0) = measured.x;
  measured_(1) = measured.y;
  if (!initialized_) {
    initialize(measured);
    initialized_ = true;
  }

  kf_.predict();
  estimated_ = kf_.correct(measured_);
}

void BallState::initialize(Point measured) {
  /* clang-format off */
  kf_.transitionMatrix = (Mat_<float>(6, 6)
  << 1,0,1,0,.5, 0,
     0,1,0,1, 0,.5,
     0,0,1,0, 1, 0,
     0,0,0,1, 0, 1,
     0,0,0,0, 1, 0,
     0,0,0,0, 0, 1);
  /* clang-format on */

  kf_.statePost.at<float>(0) = measured.x;
  kf_.statePost.at<float>(1) = measured.y;
  kf_.statePost.at<float>(2) = 0;
  kf_.statePost.at<float>(3) = 0;
  kf_.statePost.at<float>(4) = 0;
  kf_.statePost.at<float>(5) = 0;
  setIdentity(kf_.measurementMatrix);
  setIdentity(kf_.processNoiseCov, Scalar::all(1e-4));
  setIdentity(kf_.measurementNoiseCov, Scalar::all(1e1));
  setIdentity(kf_.errorCovPost, Scalar::all(1e-1));
}

void BallState::reset() {
    initialized_ = false;
    kf_.init(6, 2);
}