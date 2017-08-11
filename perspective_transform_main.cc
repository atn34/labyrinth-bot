#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "find_pink_corners.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Perspective Transform", CV_WINDOW_AUTOSIZE);

  Mat src, transformed;
  Mat transform = Mat::eye(3, 3, CV_32FC1);
  std::vector<Point2f> dst;
  dst.push_back(Point(0, 0));
  dst.push_back(Point(640, 0));
  dst.push_back(Point(640, 480));
  dst.push_back(Point(0, 480));
  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    std::vector<Point2f> src_corners;
    if (FindPinkCorners(src, &src_corners)) {
      transform = getPerspectiveTransform(src_corners, dst);
    }

    warpPerspective(src, transformed, transform, src.size());

    imshow("Perspective Transform", transformed);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }
  return 0;
}

