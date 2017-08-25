#ifndef THRESHOLD_BALL_H
#define THRESHOLD_BALL_H

#include "opencv2/opencv.hpp"

constexpr int kBallBlockSize = 16 * 2 + 3;
constexpr int kBallThreshC = 102 - 255;

class ThresholdBall {
public:
  ThresholdBall(int block_size = kBallBlockSize,
                       int ball_thresh_c = kBallThreshC)
      : block_size_(block_size), ball_thresh_c_(ball_thresh_c) {}

  cv::Mat Threshold(cv::Mat transformed) const;

private:
  int block_size_;
  int ball_thresh_c_;
};

#endif /* THRESHOLD_BALL_H */
