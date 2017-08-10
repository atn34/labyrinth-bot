#include <memory>

#include "highgui.h"

struct DeleteImage {
  /// Release the image.
  ///
  void operator()(IplImage *img) { cvReleaseImage(&img); }
};

/// Smart pointer for IplImage.
///
/// Releases the image in destructor.
using UniqueImage = std::unique_ptr<IplImage, DeleteImage>;

inline UniqueImage LoadUniqueImage(const char *filename) {
  return UniqueImage(cvLoadImage(filename));
}
