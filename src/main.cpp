#include "Realsense.h"
#include "Pipe.h"
#include "Utils.h"
#include <unistd.h>

#define RATE 250

int main(int argc, char** argv) {
    auto start_time = std::chrono::high_resolution_clock::now();
    double dt = 1.0 / RATE;
	Utils::LogFmt("Hello, World!");

    Realsense cam;
    Pipe pipe("/tmp/rsimu");

    while (1)
    {
        auto angle = cam.GetIMUVector();
    
        pipe.Write(std::to_string(angle.y));

        cam.Update();

        // Handle periodic update scheduling 
        dt = Utils::ScheduleRate(RATE, start_time);
        if (dt > 1.0 / RATE) {
            Utils::LogFmt("Robot Run overran by %f s", dt);
        }
    }

}


