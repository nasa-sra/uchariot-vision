#include <librealsense2/rs.hpp>
#include <mutex>
#include <cstring>
#include "Utils.h"

// Provided by Realsense SDK
// https://github.com/IntelRealSense/librealsense/blob/e1688cc318457f7dd57abcdbedd3398062db3009/examples/example.hpp#L36
struct RSVector3 {
    RSVector3() : x(0), y(0), z(0) {}
    RSVector3(double x, double y, double z) : x(x), y(y), z(z) {}
    double x, y, z;
    inline RSVector3 operator*(double t) { return { x * t, y * t, z * t }; }
    inline RSVector3 operator-(double t) { return { x - t, y - t, z - t }; }
    inline void operator*=(double t) { x = x * t; y = y * t; z = z * t; }
    inline void operator=(RSVector3 other) { x = other.x; y = other.y; z = other.z; }
    inline void add(double t1, double t2, double t3) { x += t1; y += t2; z += t3; }
    inline std::string str() { return Utils::StrFmt("(%f, %f, %f)", x, y, z); }
};

// Provided by Realsense SDK
// https://github.com/IntelRealSense/librealsense/blob/e1688cc318457f7dd57abcdbedd3398062db3009/examples/motion/rs-motion.cpp#L112
class RotationEstimator {
    // theta is the angle of camera rotation in x, y and z components
    RSVector3 theta;
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
    inline RSVector3 GetTheta() {
        std::lock_guard<std::mutex> lock(theta_mtx);
        return theta;
    }
};