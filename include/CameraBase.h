#pragma once
#include <Eigen/Core>
#include <opencv2/videoio.hpp>

class CameraBase {
public:

    CameraBase() : _frame(), _depthData() {}

    virtual void run() {}

    cv::Mat getFrame() { return _frame; }
    cv::Mat getDepthMap() {return _depthData;}

    virtual float getDepthAtPixel(int x, int y) {return 0.0;};
    virtual Eigen::Vector3d getPointFromPixel(int x, int y) {return {0.0, 0.0, 0.0};};
    virtual Eigen::Vector3d getPointFromPixel(int x, int y, float depth) {return {0.0, 0.0, 0.0};};

    bool getColorDepthMap() { return _depthMap; }
    void setColorDepthMap(bool in) { _depthMap = in; }

    cv::Size depthDims;

protected:

    cv::Mat _frame;

    bool _depthMap{false};
    cv::Mat _depthData;

};