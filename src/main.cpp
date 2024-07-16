#include "Pipe.h"
#include "Realsense.h"
#include "Utils.h"
#include <unistd.h>

int main(int argc, char** argv) {

    Realsense cam;
    Pipe pipe("/tmp/rs_heading");

    while (1) {
        auto angle = cam.GetIMUVector();
        float obstructionDistance = cam.GetObstructionDistance();
        
        // pipe.Write()
        pipe.Write(std::to_string(angle.y));
    }
}
