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

int main(int argc, char *argv[])
{

    argparse::ArgumentParser argparse("uChariotVision");

    argparse.add_argument("-d")
        .help("Enables display window")
        .flag();

    try {
        argparse.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << argparse;
        std::exit(1);
    }

    Display *display = nullptr;
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
    // ObjectDetector objectDetector(&cam, "../models/yolov5n.onnx", "../models/classes.txt", argparse.get<float>("-c"), argparse.get<float>("-s"), argparse.get<float>("-n"));
    ObjectDetector objectDetector(&cam, "../models/yolov5n.onnx", "../models/classes.txt", 0.8, 0.2, 0.4);

    MessageQueue messageQueue("/tmp/uchariotVision");

    while (true)
    {
        timer.start();

        cam.run();
        cv::Mat frame = cam.getFrame();

        std::vector<Detection*> detections;
        closestDetector.run(detections);
        objectDetector.run(detections);

        std::string json = "{\"detections\":[";
        for (Detection* det : detections) {
            std::cout << det->name << ": " << det->pos.x() << ", " << det->pos.y() << ", " << det->pos.z() << std::endl;
            det->draw(frame);
            json += det->toJsonStr() + ",";
            delete det;
        }
        json = json.substr(0, json.size()-1);
        json += "]}";

        messageQueue.Write(json);

        int k = cv::pollKey();
        // k -= 1048576; // I don't know why
        if (k != -1)
            std::cout << k << std::endl;

        if (k == 27)
        { // esc
            break;
        }

        switch (k)
        {
        case 's':
            display->saveFrame();
            break;
        case 'd':
        {
            bool op = cam.getColorDepthMap();
            Utils::LogFmt("Setting Depth Map %s", op ? "Off" : "On");
            cam.setColorDepthMap(!op);
        }
        break;
        default:
            break;
        }

        if (display != nullptr)
        {
            display->setFrame(cam.getFrame());
        }

        timer.stop();

        sum += timer.getTimeSec();
        every++;
        if (every == fpsDisplay)
        {
            fps = fpsDisplay / sum;
            if (display != nullptr)
                display->setFps(fps);
            every = 0;
            sum = 0.0;
        }

        if (display != nullptr)
        {
            display->showFrame();
        }
        else if (every == 0)
        {
            std::cout << "FPS: " << std::fixed << std::setprecision(1) << fps << std::endl;
        }

        timer.reset();
    }

    return 0;
}
