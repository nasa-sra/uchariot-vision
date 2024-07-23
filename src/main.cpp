#include <iostream>
#include <string>
#include <errno.h>
#include <fstream>
#include <string_view>

#include <opencv2/opencv.hpp>
#include <argparse/argparse.hpp>

#include "Utils.h"
#include "Display.h"

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

    while (true) {
        timer.start();

        cam.run();

        cv::Mat depthMap = cam.getDepthMap();
        float closest = 1e6;
        for (int row = 0; row < depthMap.rows - 4; row += 4) {
            for (int col = 0; col < depthMap.cols - 4; col += 4) {
                float pixel = depthMap.at<float>(row, col);
                if (pixel > 0 && pixel < closest) {
                    closest = pixel;
                }
            }
        }
        std::cout << closest << std::endl;

        int k = cv::pollKey();
        k -= 1048576; // I don't know why
        
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

// #include "Realsense.h"
// #include "Pipe.h"
// #include "Utils.h"
// #include <unistd.h>
// #include <thread>

// // #define TEST
// #define VERBOSE

// int main(int argc, char** argv) {

// #ifndef TEST
//     Realsense cam;
// #endif
//     Pipe pipe("/tmp/rs_heading");

// 	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
// 	cam.TareIMU(cam.GetIMU());

//     while (1)
//     {

// #ifndef TEST
//         auto angle = cam.GetIMU();

//     #ifdef VERBOSE
//         Utils::LogFmt("(%f, %f, %f)", angle.x, angle.y, angle.z);
//     #endif
//         pipe.Write(std::to_string(angle.y));
//         cam.Update();
// #else
//         pipe.Write(std::to_string(3.14));
// #endif

//     }
// }


