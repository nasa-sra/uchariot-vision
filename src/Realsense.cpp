#include "Realsense.h"
#include <librealsense2/hpp/rs_context.hpp>

bool Realsense::IsIMUValid() {
    // https://github.com/IntelRealSense/librealsense/blob/e1688cc318457f7dd57abcdbedd3398062db3009/examples/motion/rs-motion.cpp#L196
    bool found_gyro = false;
    bool found_accel = false;
    rs2::context ctx;
    for (auto dev : ctx.query_devices()) {
        Utils::LogFmt("Realsense device found: %s", dev.get_description().c_str());
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

    auto callback = [&](const rs2::frame& frame) {
        if (auto fs = frame.as<rs2::frameset>()) {
            // Try to get a frame of a depth image
            // colorFrame = fs.get_color_frame();

            rs2::depth_frame depthFrame = fs.get_depth_frame();
            colorFrame = fs.get_color_frame();

            // Get the depth frame's dimensions
            auto width = depthFrame.get_width();
            auto height = depthFrame.get_height();

            // Query the distance from the camera to the object in the center of the image
            obstructionDistance = depthFrame.get_distance(width / 2, height / 2);

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

void Realsense::OpenWindow() {
    const auto window_name = "Realsense Stream";
    std::cout << "Creating window thread" << std::endl;
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    while (cv::waitKey(1) < 0 && cv::getWindowProperty(window_name, cv::WND_PROP_AUTOSIZE) >= 0) {

        // Query frame size (width and height)
        const int w = colorFrame.get_width();
        const int h = colorFrame.get_height();

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        cv::Mat image(cv::Size(w, h), CV_8UC3, (void*)colorFrame.get_data(), cv::Mat::AUTO_STEP);

        std::cout << "Attempting to create window..." << std::endl;
        // Update the window with new data
        cv::imshow(window_name, image);
    }
}

void Realsense::Stop() {
    pipe.stop();
}