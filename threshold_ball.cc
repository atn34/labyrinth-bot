#include "threshold_ball.h"

#include "connected_components.h"

using namespace cv;

const Mat &get_ball_mask() {
  static const Mat &out = []() {
    Mat board = imread("board.png");
    auto *result = new Mat(board.rows, board.cols, CV_8U);
    for (int r = 0; r < board.rows; ++r) {
      for (int c = 0; c < board.cols; ++c) {
        const auto &point = board.at<Vec3b>(r, c);
        if (point[0] == 220 && point[1] == 0 && point[2] == 255) {
          result->at<uchar>(r, c) = 0;
        } else {
          result->at<uchar>(r, c) = 1;
        }
      }
    }
    return *result;
  }();
  return out;
}

Mat ThresholdBall::Threshold(Mat transformed) const {
  std::vector<Mat> hsv_planes;
  split(transformed, hsv_planes);

  adaptiveThreshold(hsv_planes[0], hsv_planes[0], 255,
                    CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,
                    block_size_, ball_thresh_c_);

  Mat masked;
  hsv_planes[0].copyTo(masked, get_ball_mask());

  return masked;
}
