#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "ball_state.h"
#include "measure_ball_position.h"
#include "motor_client.h"
#include "perspective_transform.h"
#include "subgoals.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, 60);
  if (!cap.isOpened())
    return -1;
  namedWindow("Controller", CV_WINDOW_AUTOSIZE);

  Mat src, transformed;

  BallState ball_state;

  Subgoals sub_goals;
  std::unique_ptr<MotorClient> motor_client;

  Point goal;
  Point accumulated_error;

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
        if (motor_client != nullptr) {
            std::cout << "perspective transform failed" << std::endl;
            motor_client.reset();
            ball_state.reset();
            sub_goals.reset();
            cvWaitKey(0);
        }
        continue;
    }

    Point measured;
    if (!MeasureBallPosition(transformed, &measured)) {
        if (motor_client != nullptr) {
            std::cout << "finding ball failed" << std::endl;
            motor_client.reset();
            ball_state.reset();
            sub_goals.reset();
            cvWaitKey(0);
        }
        continue;
    }

    if (motor_client == nullptr) {
        motor_client.reset(new MotorClient);
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

    bool ball_at_rest = vx * vx + vy * vy < 0.2 && ax * ax + ay * ay < 0.001;

    float px;
    float py;
    float dx;
    float dy;
    float ex = goal.x - x;
    float ey = goal.y - y;
    float ix;
    float iy;

    if (ball_at_rest) {
        px = 0;
        py = 0;
        dx = 0;
        dy = 0;
        ix = 0.02;
        iy = 0.03;
        accumulated_error.x += ex;
        accumulated_error.y += ey;
    } else {
        px = 1.8;
        py = 1.9;
        dx = 6;
        dy = 10;
        ix = 0;
        iy = 0;
    }

    goal = sub_goals.next_goal(Point(x, y));

    int motor_x = ex * px + accumulated_error.x * ix - vx * dx;
    int motor_y = ey * py + accumulated_error.y * iy - vy * dy;

    motor_client->step_to(MotorClient::HORIZONTAL, motor_x);
    motor_client->step_to(MotorClient::VERTICAL, motor_y);

    circle(transformed, Point(x, y), 10,
           ball_at_rest ? Scalar(255, 0, 0) : Scalar(0, 0, 255));

    circle(transformed, goal, 10, Scalar(0, 255, 0));

    imshow("Controller", transformed);
  }

  return 0;
}
