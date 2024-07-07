#pragma once

#include <librealsense2/rs.hpp>

#include "Utils.h"
#include "RotationEstimator.h"

class Realsense {

private:
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::pipeline_profile profile;

public:
    RotationEstimator algo;
    static bool IsIMUValid();

    Realsense();
    void Update();
    void Stop();
};