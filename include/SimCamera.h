
#include <opencv2/videoio.hpp>
#include <vector>
#include <fstream>

class SimCamera {
public:

    SimCamera(std::string fileName);
    ~SimCamera();

    void run();

    cv::Mat getFrame() { return _frame; }
    cv::Mat getDepthMap() {return _depthData;}
    bool getColorDepthMap() { return _depthMap; }
    void setColorDepthMap(bool in) { _depthMap = in; }

private:

    cv::Mat _frameRaw, _frame;

    float _depthScale;
    bool _depthMap{false};
    cv::Mat _depthData;

    cv::VideoCapture _cap;

};