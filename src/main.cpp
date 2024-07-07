#include "Realsense.h"
#include "Utils.h"

int main(int argc, char** argv) {
	
	Utils::LogFmt("Hello, World!");

    Realsense cam;

	// Main loop
    while (1)
    {
		Utils::LogFmt("Angle is %s", cam.algo.GetTheta().str());
        cam.Update();
    }

    cam.Stop();
}

