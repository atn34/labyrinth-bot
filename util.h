#include <memory>

#include "highgui.h"

struct DeleteImage {
  int operator()(IplImage *img) { cvReleaseImage(&img); }
};

using UniqueImage = std::unique_ptr<IplImage, DeleteImage>;

inline UniqueImage LoadUniqueImage(const char *filename) {
  return UniqueImage(cvLoadImage(filename));
}
