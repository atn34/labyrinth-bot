#include <iostream>

#include "opencv2/highgui.hpp"

#include "find_pink_corners.h"

using namespace cv;
using namespace std;

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Hue", CV_WINDOW_AUTOSIZE);

  int hue_low = kPinkHueLow;
  int hue_high = kPinkHueHigh;
  int erosion_size = kPinkErosionSize;
  createTrackbar("hue_low", "Hue", &hue_low, 255);
  createTrackbar("hue_high", "Hue", &hue_high, 255);
  createTrackbar("Kernel size:\n 2n +1", "Hue", &erosion_size,
                 21);

  Mat src;
  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    HueThresholder thresholder(hue_low, hue_high, erosion_size);

    Mat threshed = thresholder.thresh(src);

    ConnectedComponentsVisitor visitor(&threshed);

    int num_connected_components = 0;
    visitor.Visit([&](int row, int col, int label) {
      num_connected_components = label + 1;
    });
    std::cout << "num_connected_components: " << num_connected_components << std::endl;

    imshow("Hue", threshed);

    if (cvWaitKey(30) > 0) {
        break;
    }
  }
  return 0;
}