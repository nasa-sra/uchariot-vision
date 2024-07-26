
#include <opencv2/videoio.hpp>
#include <vector>
#include <fstream>

#include "CameraBase.h"

class SimCamera : public CameraBase {
public:

    SimCamera(std::string fileName);
    ~SimCamera();

    void run();

private:

    cv::VideoCapture _cap;

};
