#include <memory>

#include "highgui.h"
#include "util.h"

int main(int argc, char **argv) {
  auto capture = UniqueCaptureFromCamera(0);
  double fps = cvGetCaptureProperty(capture.get(), CV_CAP_PROP_FPS);
  cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
  for (;;) {
    IplImage *frame = cvQueryFrame(capture.get());
    auto img = CreateUniqueImage(frame->width, frame->height, IPL_DEPTH_8U, 1);
    cvCanny(frame, img.get(), 10, 100, 3);
    cvShowImage("Example1", img.get());
    int wait_ms = static_cast<int>(1000 / fps);
    int key_press = cvWaitKey(wait_ms);
    if (key_press > 0 && key_press != 33 /* escape */) {
      break;
    }
  }
  cvDestroyWindow("Example1");
}
