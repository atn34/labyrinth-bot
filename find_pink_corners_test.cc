#include <gtest/gtest.h>

#include "opencv2/highgui/highgui.hpp"

#include "find_pink_corners.h"

using namespace cv;

TEST(FourCorners, Test) {
  for (const auto &filename : {
           "./testdata/2017-08-11-000135.jpg",
           "./testdata/2017-08-11-124339.jpg",
           "./testdata/2017-08-11-124344.jpg",
           "./testdata/2017-08-11-124351.jpg",
           "./testdata/2017-08-11-124402.jpg",
           "./testdata/2017-08-11-124411.jpg",
           "./testdata/2017-08-11-124421.jpg",
           "./testdata/2017-08-11-124430.jpg",
       }) {
    Mat src = imread("./testdata/2017-08-11-124411.jpg", CV_LOAD_IMAGE_COLOR);

    HueThresholder thresholder;
    Mat threshed = thresholder.thresh(src);
    ConnectedComponentsVisitor visitor(&threshed);

    int num_connected_components = 0;
    visitor.Visit([&](int row, int col, int label) {
      num_connected_components = label + 1;
    });

    EXPECT_EQ(4, num_connected_components) << filename;
  }
}
