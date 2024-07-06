#include "Utils.h"
#include "RotationEstimator.h"

bool IsIMUValid() {
    // https://github.com/IntelRealSense/librealsense/blob/e1688cc318457f7dd57abcdbedd3398062db3009/examples/motion/rs-motion.cpp#L196
    bool found_gyro = false;
    bool found_accel = false;
	rs2::context ctx;
    for (auto dev : ctx.query_devices())
    {
	    Utils::LogFmt("Realsense device found: %s", dev.get_description().c_str());
        // The same device should support gyro and accel
        found_gyro = false;
        found_accel = false;
        for (auto sensor : dev.query_sensors())
        {
            for (auto profile : sensor.get_stream_profiles())
            {
                if (profile.stream_type() == RS2_STREAM_GYRO)
                    found_gyro = true;

                if (profile.stream_type() == RS2_STREAM_ACCEL)
                    found_accel = true;
            }
        }
        if (found_gyro && found_accel)
            break;
    }
    return found_gyro && found_accel;
}
int main(int argc, char** argv) {
	
	Utils::LogFmt("Hello, World!");

	// Before running the example, check that a device supporting IMU is connected
    if (!IsIMUValid()) {
        Utils::ErrFmt("Device supporting IMU not found");
    }

	rs2::pipeline pipe;
    // Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;

    // Add streams of gyro and accelerometer to configuration
    cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);

    // Declare object that handles camera pose calculations
    RotationEstimator algo;

    // Start streaming with the given configuration;
    // Note that since we only allow IMU streams, only single frames are produced
    auto profile = pipe.start(cfg, [&](rs2::frame frame)
    {
        // Cast the frame that arrived to motion frame
        auto motion = frame.as<rs2::motion_frame>();
        // If casting succeeded and the arrived frame is from gyro stream
        if (motion && motion.get_profile().stream_type() == RS2_STREAM_GYRO && motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
        {
            // Get the timestamp of the current frame
            double ts = motion.get_timestamp();
            // Get gyro measures
            rs2_vector gyro_data = motion.get_motion_data();
            // Call function that computes the angle of motion based on the retrieved measures
            algo.ProcessGyro(gyro_data, ts);
        }
        // If casting succeeded and the arrived frame is from accelerometer stream
        if (motion && motion.get_profile().stream_type() == RS2_STREAM_ACCEL && motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
        {
            // Get accelerometer measures
            rs2_vector accel_data = motion.get_motion_data();
            // Call function that computes the angle of motion based on the retrieved measures
            algo.ProcessAccel(accel_data);
        }
    });

	// Main loop
    while (1)
    {
		Utils::LogFmt("Angle is %s", algo.GetTheta().str());
    }
    // Stop the pipeline
    pipe.stop();
}

