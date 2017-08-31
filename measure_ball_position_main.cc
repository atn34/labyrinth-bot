#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "ball_state.h"
#include "measure_ball_position.h"
#include "perspective_transform.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, 60);
  if (!cap.isOpened())
    return -1;
  namedWindow("Ball position", CV_WINDOW_AUTOSIZE);

  Mat src, transformed;

  BallState ball_state;

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      std::cout << "perspective transform failed" << std::endl;
      break;
    }

    Point measured;
    if (!MeasureBallPosition(transformed, &measured)) {
      std::cout << "finding ball failed" << std::endl;
      break;
    }

    ball_state.update(measured);

    int key = cvWaitKey(1);
    if (key == 'q') {
      break;
    }

    float x = ball_state.position().x;
    float y = ball_state.position().y;
    float vx = ball_state.velocity().x;
    float vy = ball_state.velocity().y;
    float ax = ball_state.acceleration().x;
    float ay = ball_state.acceleration().y;

    bool ball_at_rest = vx * vx + vy * vy < 0.1 && ax * ax + ay * ay < 0.001;

    circle(transformed, Point(x, y), 10,
           ball_at_rest ? Scalar(255, 0, 0) : Scalar(0, 0, 255));

    imshow("Ball position", transformed);
  }
  return 0;
}
