#include <iostream>
#include <filesystem>

#include "Utils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "SimCamera.h"

SimCamera::SimCamera(std::string fileName) : 
        CameraBase(),
        _cap(fileName)
    {

    Utils::LogFmt("Enabling video capture");

    if (!_cap.isOpened()) {
        Utils::LogFmt("Failed to open video file at %s", fileName.c_str());
    } else {
        _cap.read(_frame);
        Utils::LogFmt("Streaming at %ix%i", _frame.cols, _frame.rows);
    }
}

SimCamera::~SimCamera() {
    _cap.release();
}

void SimCamera::run() {
    if (!_cap.read(_frame)) {
        _cap.set(cv::CAP_PROP_POS_FRAMES, 0);
        _cap.read(_frame);
    }
    if (_frame.rows <= 0) {
        Utils::LogFmt("SimCamera::run failed to get frame");
    }
}