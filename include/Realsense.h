#pragma once

#include <librealsense2/rs.hpp>

#include "Utils.h"
#include "RotationEstimator.h"

class Realsense {

private:
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::pipeline_profile profile;
    RotationEstimator algo;

public:
    static bool IsIMUValid();
    Realsense();
    void Update();
    void Stop();
    inline IMU_Vector GetIMUVector() { return algo.GetTheta(); }
};