#include <string>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "camera_properties.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, CAMERA_FPS);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
  cap.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
  if (!cap.isOpened()) return -1;

  Mat src;

  for (int iter = 0;; ++iter) {
    int64 start = getTickCount();

    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);
    cvtColor(src, src, CV_BGR2GRAY);

    resize(src, src, Size{640, 480});

    double fps = getTickFrequency() / (getTickCount() - start);

    putText(src, "FPS: " + std::to_string(fps), Point2f(20, 20),
            FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0, 255));

    imshow("Image", src);

    int key = cvWaitKey(1);
    if (key == 'q') {
      break;
    }
  }
  return 0;
}
