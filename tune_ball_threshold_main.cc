#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "perspective_transform.h"
#include "threshold_ball.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Perspective Transform", CV_WINDOW_AUTOSIZE);

  int low = 16;
  int high = 130;
  createTrackbar("low", "Perspective Transform", &low, 255);
  createTrackbar("high", "Perspective Transform", &high, 255);

  Mat src, transformed, masked;
  Mat transform = Mat::eye(3, 3, CV_32FC1);

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
        continue;
    }

    ThresholdBall threshold_ball(low * 2 + 3, high - 255);

    masked = threshold_ball.Threshold(transformed);

    imshow("Perspective Transform", masked);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }
  return 0;
}

