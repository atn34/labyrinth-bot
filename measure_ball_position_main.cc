#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "measure_ball_position.h"
#include "perspective_transform.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, 60);
  if (!cap.isOpened())
    return -1;
  namedWindow("Ball position", CV_WINDOW_AUTOSIZE);

  Mat src, transformed, masked, ball_coords;

  // Based on http://opencvexamples.blogspot.com/2014/01/kalman-filter-implementation-tracking.html
  KalmanFilter kf(/* dimension of state */ 6, /* dimension of measurement */ 2);
  /* clang-format off */
  kf.transitionMatrix = (Mat_<float>(6, 6)
   << 1,0,1,0,.5, 0,
      0,1,0,1, 0,.5,
      0,0,1,0, 1, 0,
      0,0,0,1, 0, 1,
      0,0,0,0, 1, 0,
      0,0,0,0, 0, 1);
  /* clang-format on */

  Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));

  setIdentity(kf.measurementMatrix);
  setIdentity(kf.processNoiseCov, Scalar::all(1e-4));
  setIdentity(kf.measurementNoiseCov, Scalar::all(1e1));
  kf.measurementNoiseCov.at<float>(0, 0) = 1;
  kf.measurementNoiseCov.at<float>(1, 1) = 1;
  setIdentity(kf.errorCovPost, Scalar::all(1e-1));

  kf.statePre.at<float>(0) = measurement(0);
  kf.statePre.at<float>(1) = measurement(1);
  kf.statePre.at<float>(2) = 0;
  kf.statePre.at<float>(3) = 0;
  kf.statePre.at<float>(4) = 0;
  kf.statePre.at<float>(5) = 0;

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      continue;
    }

    Point measured;
    if (!MeasureBallPosition(transformed, &measured)) {
      continue;
    }

    measurement(0) = measured.x;
    measurement(1) = measured.y;

    kf.predict();
    Mat estimated = kf.correct(measurement);
    Point statePt(estimated.at<float>(0), estimated.at<float>(1));

    float t_delta = 75 / 16; // approximate cam latency in frames
    float x = estimated.at<float>(0);
    float y = estimated.at<float>(1);
    float vx = estimated.at<float>(2);
    float vy = estimated.at<float>(3);
    float ax = estimated.at<float>(4);
    float ay = estimated.at<float>(5);
    Point currentPt(x + t_delta * vx + 0.5 * t_delta * t_delta * ax,
              y + t_delta * vy + 0.5 * t_delta * t_delta * ay);

    circle(transformed, currentPt, 10, Scalar(0, 0, 255));

    imshow("Ball position", transformed);

    if (cvWaitKey(1) > 0) {
      break;
    }
  }
  return 0;
}
