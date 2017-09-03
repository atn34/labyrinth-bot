#include <iostream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "walls_and_holes.h"

using namespace cv;

int main(int argc, char *argv[]) {
  Mat board = imread("board.png");
  Point mouse;

  cvNamedWindow("create walls", CV_WINDOW_AUTOSIZE);

  setMouseCallback(
      "create walls",
      +[](int event, int x, int y, int /* flags */, void *user_data) {
        Point &mouse = *(Point *)user_data;
        mouse.x = x;
        mouse.y = y;

        if (event == EVENT_LBUTTONDOWN) {
          std::cout << "{" << x << ", " << y << "}," << std::endl;
        }
        if (event == EVENT_RBUTTONDOWN) {
          std::cout << "},{" << std::endl;
        }
      },
      &mouse);

  for (const auto &polygon : WallPolygons()) {
    const MyPoint *last = nullptr;
    const MyPoint *first = nullptr;
    for (const auto &vertex : polygon) {
      if (last != nullptr) {
        line(board, Point(last->x, last->y), Point(vertex.x, vertex.y),
             Scalar(0, 255, 0));
      } else {
        first = &vertex;
      }
      last = &vertex;
    }
    line(board, Point(last->x, last->y), Point(first->x, first->y),
         Scalar(0, 255, 0));
  }

  for (const auto &hole : HoleCenters()) {
    circle(board, Point(hole.x, hole.y), 16, Scalar(0, 255, 0));
  }

  Mat with_cursor_circle;
  for (;;) {
    with_cursor_circle = board.clone();
    circle(with_cursor_circle, mouse, 16, Scalar(0, 0, 0));
    imshow("create walls", with_cursor_circle);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }

  return 0;
}
