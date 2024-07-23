
#include <opencv2/videoio.hpp>
#include <vector>
#include <fstream>

#include <librealsense2/rs_advanced_mode.hpp>
#include <librealsense2/rs.hpp>

class Camera {
public:

    Camera();
    ~Camera();

    void run();

    cv::Mat* getFrame() { return &_frame; }
    bool getDepthMap() { return _depthMap; }
    void setDepthMap(bool in) { _depthMap = in; }

private:

    void init();
    void loadConfig(rs2::device& dev, std::string configFile);

    cv::Mat _frameRaw, _frame;

    rs2::pipeline _pipe;
    rs2::align _align2Color;
    rs2::colorizer _colorMap;

    rs2_intrinsics _colorIntrinsics;
    rs2_extrinsics _colorExtrinsics;

    rs2_intrinsics _depthIntrinsics;
    rs2_extrinsics _depthExtrinsics;

    float _depthScale;
    bool _depthMap{false};
    cv::Mat _depthData;

    cv::VideoCapture* _simCam{nullptr};

};