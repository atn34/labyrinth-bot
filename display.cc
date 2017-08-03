#include <memory>

#include "highgui.h"
#include "util.h"

int
main(int argc, char **argv)
{
    auto img = LoadUniqueImage("labyrinth.png");
    cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
    cvShowImage("Example1", img.get());
    cvWaitKey(0);
    cvDestroyWindow("Example1");
}
