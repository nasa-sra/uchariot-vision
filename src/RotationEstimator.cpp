#include "RotationEstimator.h"

void RotationEstimator::ProcessGyro(rs2_vector gyro_data, double ts) {
    // On the first iteration, use only data from accelerometer to set the camera's initial position
    if (firstGyro) { 
        firstGyro = false;
        last_ts_gyro = ts;
        return;
    }
    // Holds the change in angle, as calculated from gyro
    IMU_Vector gyro_angle;

    // Initialize gyro_angle with data from gyro
    gyro_angle.x = gyro_data.x; // Pitch
    gyro_angle.y = gyro_data.y; // Yaw
    gyro_angle.z = gyro_data.z; // Roll

    // Compute the difference between arrival times of previous and current gyro frames
    double dt_gyro = (ts - last_ts_gyro) / 1000.0;
    last_ts_gyro = ts;

    // Change in angle equals gyro measures * time passed since last measurement
    gyro_angle = gyro_angle * static_cast<float>(dt_gyro);

    // Apply the calculated change of angle to the current angle (theta)
    std::lock_guard<std::mutex> lock(theta_mtx);
    theta.add(-gyro_angle.z, -gyro_angle.y, gyro_angle.x);
}

void RotationEstimator::ProcessAccel(rs2_vector accel_data) {
    // Holds the angle as calculated from accelerometer data
    IMU_Vector accel_angle;

    // Calculate rotation angle from accelerometer data
    accel_angle.z = std::atan2(accel_data.y, accel_data.z);
    accel_angle.x = std::atan2(accel_data.x, sqrt(accel_data.y * accel_data.y + accel_data.z * accel_data.z));

    // If it is the first iteration, set initial pose of camera according to accelerometer data (note the different handling for Y axis)
    std::lock_guard<std::mutex> lock(theta_mtx);
    if (firstAccel) {
        firstAccel = false;
        theta = accel_angle;
        // Since we can't infer the angle around Y axis using accelerometer data, we'll use PI as a convetion for the initial pose
        theta.y = M_PI;
    } else {
        /* 
        Apply Complementary Filter:
            - high-pass filter = theta * alpha:  allows short-duration signals to pass through while filtering out signals
                that are steady over time, is used to cancel out drift.
            - low-pass filter = accel * (1- alpha): lets through long term changes, filtering out short term fluctuations 
        */
        theta.x = theta.x * alpha + accel_angle.x * (1 - alpha);
        theta.z = theta.z * alpha + accel_angle.z * (1 - alpha);
    }
}