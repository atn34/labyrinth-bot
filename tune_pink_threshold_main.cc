#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Hue", CV_WINDOW_AUTOSIZE);

  int hue_low = 4;
  int hue_high = 16;
  int erosion_elem = 0;
  int erosion_size = 3;
  createTrackbar("hue_low", "Hue", &hue_low, 255);
  createTrackbar("hue_high", "Hue", &hue_high, 255);
  /// Create Erosion Trackbar
  createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Hue",
                 &erosion_elem, 2);

  createTrackbar("Kernel size:\n 2n +1", "Hue", &erosion_size,
                 21);

  Mat src;
  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    cvtColor(src, src, CV_BGR2HSV);

    /// Separate the image in 3 places ( B, G and R )
    vector<Mat> hsv_planes;
    split(src, hsv_planes);

    if (cvWaitKey(30) > 0) {
        break;
    }
    inRange(hsv_planes[0], hue_low, hue_high, hsv_planes[0]);

    int erosion_type;
    if (erosion_elem == 0) {
      erosion_type = MORPH_RECT;
    } else if (erosion_elem == 1) {
      erosion_type = MORPH_CROSS;
    } else if (erosion_elem == 2) {
      erosion_type = MORPH_ELLIPSE;
    }

    Mat element = getStructuringElement(
        erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));

    /// Apply the erosion operation
    erode(hsv_planes[0], hsv_planes[0], element);

    imshow("Hue", hsv_planes[0]);

  }
  return 0;
}