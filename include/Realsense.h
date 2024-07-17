#pragma once

#include "RotationEstimator.h"
#include "Utils.h"
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

class Realsense {

private:
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::frame frame;
    rs2::video_frame colorFrame = rs2::video_frame(frame);

    RotationEstimator algo;
    float obstructionDistance;

public:
    static bool IsIMUValid();
    Realsense();
    void Stop();
    void OpenWindow();
    inline float GetObstructionDistance() {
        return obstructionDistance;
    }
    inline IMU_Vector GetIMUVector() {
        return algo.GetTheta();
    }
};