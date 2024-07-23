
#include <opencv2/videoio.hpp>
#include <librealsense2/rs.hpp>
#include <vector>
#include <fstream>

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

    cv::VideoCapture _cap;

    cv::Mat _frameRaw, _greyFrame, _frame;

    rs2::pipeline _pipe;
    rs2::align _align2Color;
    rs2::colorizer _colorMap;

    rs2_intrinsics _colorIntrinsics;
    rs2_extrinsics _colorExtrinsics;

    rs2_intrinsics _depthIntrinsics;
    rs2_extrinsics _depthExtrinsics;

    float _depthScale;
    bool _depthMap;

};