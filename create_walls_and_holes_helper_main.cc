#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "camera_properties.h"
#include "subgoals.h"
#include "walls_and_holes.h"

using namespace cv;

struct MouseState {
  Point mouse;
  Point upper_left;
  bool selecting_rectangle = false;
};

int main(int argc, char *argv[]) {
  Mat board = imread("board.png");
  Size size(CAMERA_WIDTH, CAMERA_HEIGHT);
  resize(board, board, size);

  cvNamedWindow("create walls", CV_WINDOW_AUTOSIZE);

  MouseState mouse_state;

  setMouseCallback(
      "create walls",
      +[](int event, int x, int y, int /* flags */, void *user_data) {
        MouseState &mouse_state = *(MouseState *)user_data;
        mouse_state.mouse.x = x;
        mouse_state.mouse.y = y;
        if (event == EVENT_LBUTTONDOWN) {
          if (mouse_state.selecting_rectangle) {
            mouse_state.selecting_rectangle = false;
            std::cout << "{";
            for (const Point &point :
                 {mouse_state.upper_left, Point(x, mouse_state.upper_left.y),
                  mouse_state.mouse, Point(mouse_state.upper_left.x, y)}) {
              std::cout << "{" << point.x << "," << point.y << "},";
            }
            std::cout << "}," << std::endl;
          } else {
            mouse_state.selecting_rectangle = true;
            mouse_state.upper_left = Point(x, y);
          }
        }
        if (event == EVENT_RBUTTONDOWN) {
          std::cout << "{" << x << "," << y << "}," << std::endl;
        }
      },
      &mouse_state);

  for (const auto &polygon : WallPolygons()) {
    const Vec2 *last = nullptr;
    const Vec2 *first = nullptr;
    for (const auto &vertex : polygon) {
      if (last != nullptr) {
        line(board, Point(last->x, last->y), Point(vertex.x, vertex.y),
             Scalar(0, 255, 0));
      } else {
        first = &vertex;
      }
      last = &vertex;
    }
    if (last != nullptr) {
      line(board, Point(last->x, last->y), Point(first->x, first->y),
           Scalar(0, 255, 0));
    }
  }

  for (const auto &hole : HoleCenters()) {
    circle(board, Point(hole.x, hole.y), kHoleRadius, Scalar(0, 255, 0));
  }

  Subgoals subgoals;

  Mat with_cursors;
  for (;;) {
    with_cursors = board.clone();
    Vec2 mouse = Vec2FromInt(mouse_state.mouse.x, mouse_state.mouse.y);
    if (mouse_state.selecting_rectangle) {
      rectangle(with_cursors, mouse_state.upper_left, mouse_state.mouse,
                Scalar(0, 0, 0));
    } else {
      bool touching_obstacle;
      Vec2 subgoal = subgoals.next_goal(
          mouse, &touching_obstacle, [&](float theta, float length) {
            Vec2 target = mouse + UnitVec2FromTheta(theta) * length;
            line(with_cursors, Point(mouse.x, mouse.y),
                 Point(target.x, target.y), Scalar(0, 0, 0));
          });
      circle(with_cursors, Point(mouse.x, mouse.y), kBallRadius,
             touching_obstacle ? Scalar(255, 255, 255) : Scalar(0, 0, 0));
      circle(with_cursors, Point(subgoal.x, subgoal.y), kBallRadius,
             Scalar(0, 255, 0));
    }
    imshow("create walls", with_cursors);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }

  return 0;
}
