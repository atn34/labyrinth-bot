#include <iostream>

#include "opencv2/highgui.hpp"

#include "camera_properties.h"
#include "perspective_transform.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, CAMERA_FPS);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
  cap.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
  if (!cap.isOpened()) return -1;
  namedWindow("Perspective Transform", CV_WINDOW_AUTOSIZE);

  Mat src, transformed;

  for (;;) {
    int key = cvWaitKey(1);
    if (key == 'q') {
      break;
    }

    int64 start = getTickCount();

    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      std::cerr << "perspective transform failed" << std::endl;
      break;
    }

    double fps = getTickFrequency() / (getTickCount() - start);
    putText(transformed, "FPS: " + std::to_string(fps), Point2f(20, 20),
            FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255, 255));

    imshow("Perspective Transform", transformed);
  }
}
