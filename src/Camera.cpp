#include <iostream>
#include <filesystem>

#include "Utils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Camera.h"

Camera::Camera() : 
        _frame(), 
        _frameRaw(), 
        _align2Color(RS2_STREAM_COLOR),
        _depthData()
    {

    Utils::LogFmt("Enabling video capture");

    try {
        init();
    } catch (const rs2::error & e) {
        std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
        init();
    }

    rs2::video_stream_profile colorProfile = _pipe.get_active_profile().get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
    rs2::video_stream_profile depthProfile = _pipe.get_active_profile().get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();

    _colorIntrinsics = colorProfile.get_intrinsics();
    _colorExtrinsics = colorProfile.get_extrinsics_to(depthProfile);
    
    _depthIntrinsics = depthProfile.get_intrinsics();
    _depthExtrinsics = depthProfile.get_extrinsics_to(colorProfile);

    Utils::LogFmt("Streaming at %ix%i", colorProfile.width(), colorProfile.height());
}

Camera::~Camera() {
    _pipe.stop();
}

void Camera::init() {
    rs2::context ctx;
    rs2::device_list devices = ctx.query_devices();
    rs2::device dev = devices[0];

	Utils::LogFmt("Realsense device found: %s", dev.get_description().c_str());

    std::string serial = dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

    std::string json_file_name = "rsConfig.json";
    if (std::filesystem::exists(json_file_name)) {
        loadConfig(dev, json_file_name);
    }

    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR);
    cfg.enable_stream(RS2_STREAM_DEPTH);
    cfg.enable_device(serial);
    rs2::pipeline_profile pipeProfile = _pipe.start(cfg);

    rs2::depth_sensor depth_sensor = pipeProfile.get_device().first<rs2::depth_sensor>();
    _depthScale = depth_sensor.get_depth_scale();
}

void Camera::loadConfig(rs2::device& dev, std::string configFile) {
    auto advanced_mode_dev = dev.as<rs400::advanced_mode>();
    // Check if advanced-mode is enabled to pass the custom config
    if (!advanced_mode_dev.is_enabled()) {
        // If not, enable advanced-mode
        advanced_mode_dev.toggle_advanced_mode(true);
        std::cout << "Advanced mode enabled. " << std::endl;
    }

    std::ifstream t(configFile);
    std::string preset_json((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    advanced_mode_dev.load_json(preset_json);
}

void Camera::run() {
    try {
        rs2::frameset data = _pipe.wait_for_frames();

        // rs2::frame irFrame = data.get_infrared_frame();
        rs2::frame colorFrame = data.get_color_frame();
        rs2::depth_frame depthFrame = data.get_depth_frame();
        rs2::depth_frame alignedDepthFrame = _align2Color.process(depthFrame).as<rs2::depth_frame>();
        _depthData = cv::Mat(cv::Size(alignedDepthFrame.as<rs2::video_frame>().get_width(), alignedDepthFrame.as<rs2::video_frame>().get_height()), CV_32FC1, (void*)alignedDepthFrame.get_data(), cv::Mat::AUTO_STEP);

        int width = colorFrame.as<rs2::video_frame>().get_width();
        int height = colorFrame.as<rs2::video_frame>().get_height();

        _frameRaw = cv::Mat(cv::Size(width, height), CV_8UC3, (void*)colorFrame.get_data(), cv::Mat::AUTO_STEP);
        cv::cvtColor(_frameRaw, _frame, cv::COLOR_RGB2BGR);
        if (_depthMap) {
            rs2::frame depthFrameColor = _colorMap.colorize(depthFrame);
            _frame = cv::Mat(cv::Size(depthFrameColor.as<rs2::video_frame>().get_width(), depthFrameColor.as<rs2::video_frame>().get_height()), CV_8UC3, (void*)depthFrameColor.get_data(), cv::Mat::AUTO_STEP);
        }

    } catch (const rs2::error & e) {
        std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    }
}