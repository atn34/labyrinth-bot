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

inline UniqueImage CreateUniqueImage(int width, int height, int depth,
                                     int nChannels) {
  IplImage *img = cvCreateImage(cvSize(width, height), depth, nChannels);
  return UniqueImage(img);
}

struct DeleteCapture {
  /// Release the capture.
  ///
  void operator()(CvCapture *cap) { cvReleaseCapture(&cap); }
};

/// Smart pointer for CvCapture.
///
/// Releases the capture in destructor.
using UniqueCapture = std::unique_ptr<CvCapture, DeleteCapture>;

inline UniqueCapture UniqueCaptureFromCamera(int cam) {
  return UniqueCapture(cvCreateCameraCapture(cam));
}
