#include "Pipe.h"
#include "Realsense.h"
#include "Utils.h"
#include <unistd.h>

int main(int argc, char** argv) {

    Realsense cam;
    // Pipe pipe("/tmp/rs_heading");

    while (1) {
        cam.Update();
        auto angle = cam.GetIMUVector();
        
    
      //  std::cout << "X: " << angle.x << "Y: " << angle.y << "Z: " << angle.z << std::endl;
        // Utils::LogFmt("(%f, %f, %f)", angle.x, angle.y, angle.z);
        // pipe.Write(std::to_string(angle.y));
    }
}
