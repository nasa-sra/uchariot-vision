#pragma once

#include <opencv2/videoio.hpp>
#include <vector>
#include <fstream>

#include <librealsense2/rs_advanced_mode.hpp>
#include <librealsense2/rs.hpp>
#include <Eigen/Core>

class CameraBase {
public:

    CameraBase() : _frame(), _depthData() {}

    virtual void run() {}

    cv::Mat getFrame() { return _frame; }
    cv::Mat getDepthMap() {return _depthData;}
    virtual Eigen::Vector3d getCameraPoint(int x, int y) {return {0.0, 0.0, 0.0};}

    bool getColorDepthMap() { return _depthMap; }
    void setColorDepthMap(bool in) { _depthMap = in; }

    cv::Size depthDims;

protected:

    cv::Mat _frame;

    bool _depthMap{false};
    cv::Mat _depthData;

};

class Camera : public CameraBase {
public:

    Camera();
    ~Camera();

    void run();
    Eigen::Vector3d getCameraPoint(int x, int y);

private:

    void init();
    void loadConfig(rs2::device& dev, std::string configFile);

    cv::Mat _frameRaw;

    rs2::pipeline _pipe;
    rs2::align _align2Color;
    rs2::colorizer _colorMap;

    rs2_intrinsics _colorIntrinsics;
    rs2_extrinsics _colorExtrinsics;

    rs2_intrinsics _depthIntrinsics;
    rs2_extrinsics _depthExtrinsics;

    float _depthScale;

};

