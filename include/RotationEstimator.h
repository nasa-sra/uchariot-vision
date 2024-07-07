#pragma once

#include <librealsense2/rs.hpp>
#include <mutex>
#include <cstring>
#include "Utils.h"

#define TWO_PI 6.28318530717958

struct IMU_Vector {
    inline static double constrain(double t) { return std::fmod(t + TWO_PI, TWO_PI); }
    IMU_Vector() : x(0), y(0), z(0) {}
    IMU_Vector(double x, double y, double z) : x(x), y(y), z(z) {}
    double x, y, z;
    inline IMU_Vector operator*(double t) { return { x * t, y * t, z * t }; }
    inline IMU_Vector operator-(double t) { return { x - t, y - t, z - t }; }
    inline void operator*=(double t) { x = x * t; y = y * t; z = z * t; }
    inline void operator=(IMU_Vector other) { x = other.x; y = other.y; z = other.z; }
    inline void add(double t1, double t2, double t3) { x += t1; y += t2; z += t3; }
    inline std::string str() { return Utils::StrFmt("(%f, %f, %f)", x, y, z); }
    inline IMU_Vector constrained() { return IMU_Vector(constrain(x), constrain(y), constrain(z)); }
};

class RotationEstimator {
    // theta is the angle of camera rotation in x, y and z components
    IMU_Vector theta;
    std::mutex theta_mtx;
    /* alpha indicates the part that gyro and accelerometer take in computation of theta; higher alpha gives more weight to gyro, but too high
    values cause drift; lower alpha gives more weight to accelerometer, which is more sensitive to disturbances */
    double alpha = 0.98f;
    bool firstGyro = true;
    bool firstAccel = true;
    // Keeps the arrival time of previous gyro frame
    double last_ts_gyro = 0;
public:
    // Function to calculate the change in angle of motion based on data from gyro
    void ProcessGyro(rs2_vector gyro_data, double ts);

    void ProcessAccel(rs2_vector accel_data);
   
    // Returns the current rotation angle
    inline IMU_Vector GetTheta() {
        std::lock_guard<std::mutex> lock(theta_mtx);
        auto t = theta.constrained();
        t.y = TWO_PI + t.y;
        return t;
    }
};