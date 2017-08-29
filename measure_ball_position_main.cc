#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "measure_ball_position.h"
#include "motor_client.h"
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
  Mat_<float> control(2,1); measurement.setTo(Scalar(0));

  setIdentity(kf.measurementMatrix);
  setIdentity(kf.processNoiseCov, Scalar::all(1e-4));
  setIdentity(kf.measurementNoiseCov, Scalar::all(1e1));
  kf.measurementNoiseCov.at<float>(0, 0) = 4;
  kf.measurementNoiseCov.at<float>(1, 1) = 4;
  setIdentity(kf.errorCovPost, Scalar::all(1e-1));

  MotorClient motor_client;

  float x_goal = 187;
  float y_goal = 15;

  float pending_x_steps = 0.0;
  float pending_y_steps = 0.0;

  int pause_motors_until_iter = 100;
  for (int iter = 0;; ++iter) {
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

    measurement(0) = measured.x;
    measurement(1) = measured.y;

    kf.predict();
    Mat estimated = kf.correct(measurement);

    int key = cvWaitKey(1);
    if (key == 'q') {
      break;
    }

    float x = estimated.at<float>(0);
    float y = estimated.at<float>(1);
    float vx = estimated.at<float>(2);
    float vy = estimated.at<float>(3);
    float ax = estimated.at<float>(4);
    float ay = estimated.at<float>(5);

    bool ball_at_rest = vx * vx + vy * vy < 0.1 && ax * ax + ay * ay < 0.001;

    float p = 0.01;
    float d = 0;
    float dd = 0;

    if (iter >= pause_motors_until_iter) {
      pending_x_steps -= (x - x_goal) * p + vx * d + ax * dd;
      pending_y_steps -= (y - y_goal) * p + vy * d + ay * dd;

      control(0) = 0;
      control(1) = 0;
      if (floor(std::abs(pending_x_steps)) > 0) {
        control(0) = floor(pending_x_steps);
        pending_x_steps -= floor(pending_x_steps);
      }

      if (floor(std::abs(pending_y_steps)) > 0) {
        control(1) = floor(pending_y_steps);
        pending_y_steps -= floor(pending_y_steps);
      }

      pause_motors_until_iter = std::max(std::abs(control(0)), std::abs(control(1)));

      motor_client.step(MotorClient::HORIZONTAL, control(0));
      motor_client.step(MotorClient::VERTICAL, control(1));
    }

    circle(transformed, Point(x, y), 10,
           ball_at_rest ? Scalar(255, 0, 0) : Scalar(0, 0, 255));
    circle(transformed, Point(x_goal, y_goal), 10, Scalar(0, 255, 0));

    imshow("Ball position", transformed);
  }
  return 0;
}
