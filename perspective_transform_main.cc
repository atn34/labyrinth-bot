#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "find_pink_corners.h"

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

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Perspective Transform", CV_WINDOW_AUTOSIZE);

  int low = 16;
  int high = 102;
  createTrackbar("low", "Perspective Transform", &low, 255);
  createTrackbar("high", "Perspective Transform", &high, 255);

  Mat src, transformed, masked;
  Mat transform = Mat::eye(3, 3, CV_32FC1);
  std::vector<Point2f> dst;
  dst.push_back(Point(10, 10));
  dst.push_back(Point(630, 10));
  dst.push_back(Point(630, 470));
  dst.push_back(Point(10, 470));

  Mat mask = get_ball_mask();

  Mat result = Mat::zeros(480, 640, CV_8UC1);

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    std::vector<Point2f> src_corners;
    if (FindPinkCorners(src, &src_corners)) {
      transform = getPerspectiveTransform(src_corners, dst);
    }

    warpPerspective(src, transformed, transform, src.size());

    Mat hsv;

    std::vector<Mat> hsv_planes;
    split(transformed, hsv_planes);

    adaptiveThreshold(hsv_planes[0], hsv_planes[0], 255,
                      CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,
                      low * 2 + 3, high - 255);

    hsv_planes[0].copyTo(masked, mask);

    bitwise_or(result, masked, result);
    imshow("Perspective Transform", result);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }
  return 0;
}

