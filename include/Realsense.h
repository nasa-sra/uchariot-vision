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
	IMU_Vector tare;

public:
    static bool IsIMUValid();
    Realsense();
    void Update();
    void Stop();
    inline IMU_Vector GetIMU() { return (algo.GetTheta() - tare).constrained(); }
	inline void TareIMU(IMU_Vector tare) { this->tare = tare; }
};
