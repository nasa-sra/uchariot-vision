#include "Realsense.h"
#include "Pipe.h"
#include "Utils.h"
#include <unistd.h>

// #define TEST
#define VERBOSE

int main(int argc, char** argv) {

#ifndef TEST
    Realsense cam;
#endif
    Pipe pipe("/tmp/rs_heading");

    while (1)
    {

#ifndef TEST
        auto angle = cam.GetIMUVector();

    #ifdef VERBOSE
        Utils::LogFmt("(%f, %f, %f)", angle.x, angle.y, angle.z);
    #endif
        pipe.Write(std::to_string(angle.y));
        cam.Update();
#else
        pipe.Write(std::to_string(3.14));
#endif

    }
}


