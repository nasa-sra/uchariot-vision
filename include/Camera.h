
#include "opencv2/videoio.hpp"
#include <librealsense2/rs.hpp>
#include <vector>
#include <fstream>

class Camera {
public:

    Camera();
    ~Camera();

    void run();
    void printPoses();
    void runTest();
    void saveData();
    void destroyDetections();

    zarray_t* getDetections() {
        if (_detections != nullptr) {
            return _detections;
        }
        return nullptr;
    }

    int getDetectionsSize() {
        if (_detections != nullptr) {
            return zarray_size(_detections);
        }
        return -1;
    }

    cv::Mat* getFrame() {
        return &_frame;
    }

    std::vector<Pose> getPoses() {
        return _poses;
    }

    void setYCrop(int yCrop = 150) {
        _yCrop = yCrop;
    }

private:

    void init();

    zarray_t* _detections;
    std::vector<Pose> _poses;

    cv::VideoCapture _cap;

    cv::Mat _frameRaw, _greyFrame, _frame;

    std::string _testData;

    rs2::pipeline _pipe;
    rs2::align _align2Color;
    rs2::colorizer _colorMap;

    rs2_intrinsics _colorIntrinsics;
    rs2_extrinsics _colorExtrinsics;

    rs2_intrinsics _depthIntrinsics;
    rs2_extrinsics _depthExtrinsics;

    float _depthScale;

    bool _depthMap;

    int _yCrop = 150;

};