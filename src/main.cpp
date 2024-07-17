#include "Pipe.h"
#include "Realsense.h"
#include "Utils.h"
#include <thread>
#include <unistd.h>

std::thread windowThread;

int main(int argc, char** argv) {

    Realsense cam;
    Pipe pipe("/tmp/rs_heading");

    while (1) {
        if (argv[1]) {
            windowThread = std::thread([&cam]() { cam.OpenWindow(); });
        }

        auto angle = cam.GetIMUVector();
        float obstructionDistance = cam.GetObstructionDistance();

        // pipe.Write()
        // pipe.Write(std::to_string(angle.y));
    }

    if (windowThread.joinable()) {
        windowThread.join();
        std::cout << "Window thread shut down\n";
    }
}
