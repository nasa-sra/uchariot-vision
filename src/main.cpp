#include "Realsense.h"
#include "Pipe.h"
#include "Utils.h"
#include <unistd.h>
#include <thread>

// #define TEST
#define VERBOSE

int main(int argc, char** argv) {

#ifndef TEST
    Realsense cam;
#endif
    Pipe pipe("/tmp/rs_heading");

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	cam.TareIMU(cam.GetIMU());

    while (1)
    {

#ifndef TEST
        auto angle = cam.GetIMU();

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


