#include <iostream>
#include <string>
#include <errno.h>
#include <fstream>
#include <string_view>

#include <opencv2/opencv.hpp>
#include <argparse/argparse.hpp>

#include "Utils.h"
#include "Display.h"
#include "Detector.h"
#include "MessageQueue.h"

#ifndef SIMULATION
#include "Camera.h"
#else
#include "SimCamera.h"
#endif

int main(int argc, char *argv[]) {

    argparse::ArgumentParser argparse("uChariotVision");

    argparse.add_argument("-d")
        .help("Enables display window")
        .flag();

    try {
        argparse.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << argparse;
        std::exit(1);
    }

    Display* display = nullptr;
    if (argparse.get<bool>("-d")) {
        display = new Display(640, 480);
    }

#ifndef SIMULATION
    Camera cam;
#else
    SimCamera cam("test.mp4");
#endif

    cv::TickMeter timer;
    float fps = 0.0;
    int every = 0;
    double sum = 0.0;
    const int fpsDisplay = 10;

    ClosestDetector closestDetector(&cam);

    MessageQueue messageQueue("tmp/rs");

    while (true) {
        timer.start();

        cam.run();
        cv::Mat frame = cam.getFrame();

        std::vector<Detection> detections;
        std::vector<Detection> closestDetections = closestDetector.run();
        detections.insert(detections.end(), closestDetections.begin(), closestDetections.end());

        for (Detection& det : detections) {
            std::cout << det.name << ": " << det.pos.x() << ", " << det.pos.y() << ", " << det.pos.z() << std::endl;
            cv::circle(frame, cv::Point(det.pixelX, det.pixelY), 10, cv::Scalar(255, 255, 255), 5);
            putText(frame, det.name, cv::Point2i(det.pixelX-15, det.pixelY-10), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 3);
        }

        int k = cv::pollKey();
        // k -= 1048576; // I don't know why
        if (k != -1)
        std::cout << k << std::endl;
        
        if (k == 27) { // esc
            break;
        }

        switch (k) {
            case 's':
                display->saveFrame();
                break;
            case 'd':
                {bool op = cam.getColorDepthMap();
                Utils::LogFmt("Setting Depth Map %s", op ? "Off" : "On");
                cam.setColorDepthMap(!op);}
                break;
            default: break;
        }
        
        if (display != nullptr) {
            display->setFrame(cam.getFrame());
        }

        timer.stop();

        sum += timer.getTimeSec();
        every++;
        if (every == fpsDisplay) {
            fps = fpsDisplay / sum;
            if (display != nullptr) display->setFps(fps);
            every = 0;
            sum = 0.0;
        }

        if (display != nullptr) {
            display->showFrame();
        } else if (every == 0) {
            std::cout << "FPS: " << std::fixed << std::setprecision(1) << fps << std::endl;
        }
        
        timer.reset();
    }

    return 0;
}
