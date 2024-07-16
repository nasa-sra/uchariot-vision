#include "Realsense.h"
#include <librealsense2/hpp/rs_context.hpp>

bool Realsense::IsIMUValid() {
    // https://github.com/IntelRealSense/librealsense/blob/e1688cc318457f7dd57abcdbedd3398062db3009/examples/motion/rs-motion.cpp#L196
    bool found_gyro = false;
    bool found_accel = false;
    rs2::context ctx;
    for (auto dev : ctx.query_devices()) {
        Utils::LogFmt("Realsense device found: %s", dev.get_description().c_str());
        // The same device should support gyro and accel
        found_gyro = false;
        found_accel = false;
        for (auto sensor : dev.query_sensors()) {
            for (auto profile : sensor.get_stream_profiles()) {
                if (profile.stream_type() == RS2_STREAM_GYRO) found_gyro = true;

                if (profile.stream_type() == RS2_STREAM_ACCEL) found_accel = true;
            }
        }
        if (found_gyro && found_accel) break;
    }
    return found_gyro && found_accel;
}

Realsense::Realsense() {
    if (!IsIMUValid()) { Utils::ErrFmt("Device supporting IMU not found"); }

    cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);

    rs2::colorizer color_map;
    rs2::frame colorFrame;
    rs2::frame depthFrame;
    std::mutex theta_img;

    auto callback = [&](const rs2::frame& frame) {
        if (auto fs = frame.as<rs2::frameset>()) {
            // Try to get a frame of a depth image
            rs2::depth_frame depth = fs.get_depth_frame();
            // Get the depth frame's dimensions
            auto width = depth.get_width();
            auto height = depth.get_height();

            // Query the distance from the camera to the object in the center of the image
            float dist_to_center = depth.get_distance(width / 2, height / 2);

            // Print the distance
            std::cout << "The camera is facing an object " << dist_to_center << " meters away \r";

        } else if (frame.as<rs2::motion_frame>()) {
            // Cast the frame that arrived to motion frame
            auto motion = frame.as<rs2::motion_frame>();
            // If casting succeeded and the arrived frame is from gyro stream
            if (motion && motion.get_profile().stream_type() == RS2_STREAM_GYRO &&
                motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F) {
                // Get the timestamp of the current frame
                double ts = motion.get_timestamp();
                // Get gyro measures
                rs2_vector gyro_data = motion.get_motion_data();
                // Call function that computes the angle of motion based on the retrieved measures
                algo.ProcessGyro(gyro_data, ts);
            }
            // If casting succeeded and the arrived frame is from accelerometer stream
            if (motion && motion.get_profile().stream_type() == RS2_STREAM_ACCEL &&
                motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F) {
                // Get accelerometer measures
                rs2_vector accel_data = motion.get_motion_data();
                // Call function that computes the angle of motion based on the retrieved measures
                algo.ProcessAccel(accel_data);
            }
        }
    };

    pipe.start(cfg, callback);
}

void Realsense::Update() {
    // rs2::frameset frames = pipe.wait_for_frames();

    // float obstacleDistance = GetObstacleDistance(frames);
    // UpdateGyroAccel(frames);
}

float Realsense::GetObstacleDistance(rs2::frameset frames) {

    return -1;
}

void Realsense::UpdateGyroAccel(rs2::frameset frames) {
    auto motion = frames.as<rs2::motion_frame>();
    // auto motion = frames.get_

    // If casting succeeded and the arrived frame is from gyro stream
    if (motion) {
        std::cout << "getting motion data..." << std::endl;
        // Get the timestamp of the current frame
        double ts = motion.get_timestamp();
        // Get gyro measures
        rs2_vector gyro_data = motion.get_motion_data();
        // Call function that computes the angle of motion based on the retrieved measures
        algo.ProcessGyro(gyro_data, ts);

        // Get accelerometer measures
        rs2_vector accel_data = motion.get_motion_data();
        // Call function that computes the angle of motion based on the retrieved measures
        algo.ProcessAccel(accel_data);
    }
}

void Realsense::Stop() {
    pipe.stop();
}