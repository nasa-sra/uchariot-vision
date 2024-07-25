
#include <opencv2/videoio.hpp>
#include <vector>
#include <fstream>

#include "Camera.h"

class SimCamera : public CameraBase {
public:

    SimCamera(std::string fileName);
    ~SimCamera();

    void run();

private:

    cv::VideoCapture _cap;

};
