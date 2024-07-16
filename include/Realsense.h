#pragma once

#include <librealsense2/rs.hpp>

#include "RotationEstimator.h"
#include "Utils.h"

class Realsense {

private:
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::pipeline_profile profile;
    RotationEstimator algo;
    float obstructionDistance;
public:
    static bool IsIMUValid();
    Realsense();
    void Stop();
    inline float GetObstructionDistance() {
        return obstructionDistance;
    }
    inline IMU_Vector GetIMUVector() {
        return algo.GetTheta();
    }
};