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

public:
    static bool IsIMUValid();
    Realsense();
    void Update();
    float GetObstacleDistance(rs2::frameset frames);
    void UpdateGyroAccel(rs2::frameset frames);
    void Stop();
    inline IMU_Vector GetIMUVector() {
        return algo.GetTheta();
    }
};