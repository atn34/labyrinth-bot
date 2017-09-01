#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "perspective_transform.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Masked", CV_WINDOW_AUTOSIZE);

  int low = 16;
  int high = 130;
  createTrackbar("low", "Masked", &low, 255);
  createTrackbar("high", "Masked", &high, 255);

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

    cvtColor(transformed, transformed, CV_RGB2GRAY);

    adaptiveThreshold(transformed, transformed, 255,
                      CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,
                      2 * low + 3, 130 - high);

    imshow("Masked", transformed);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }
  imwrite("masked.png", transformed);
  return 0;
}


