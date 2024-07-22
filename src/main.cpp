#include <iostream>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fstream>
#include <string_view>

#include <opencv2/opencv.hpp>

#include "Display.h"
#include "Camera.h"

int main(int argc, char *argv[]) {

    getopt_t* getopt = getopt_create();

    getopt_add_int(getopt, 's', "show", "1", "Show video stream");

    if (!getopt_parse(getopt, argc, argv, 1)) {
        printf("Usage: %s [options]\n", argv[0]);
        getopt_do_usage(getopt);
        exit(0);
    }

    if (getopt_get_int(getopt, "show") == 1) {
        display = new Display();
    }

    Display display(640, 480);
    Camera cam;

    cv::TickMeter timer;
    float fps = 0.0;
    int every = 0;
    double sum = 0.0;
    const int fpsDisplay = 10;

    bool logging = false;
    std::string log = "";
    bool takeMean = false;
    int meanCounter;
    Point meanPos;

    int heartbeat = 0;

    while (true) {
        timer.start();

        detector->run();
        if (detector->getDetectionsSize() > 0) {
            locator->run(detector->getPoses(), 0.0);
        }

        if (ntCam->GetBoolean("feedback", false)) {
            detector->setYCrop(0);
        } else {
            detector->setYCrop();
        }

        int k = cv::pollKey();
        
        if (k == 27) { // esc
            break;
        }

        switch (k) {
            case ' ':
                detector->printPoses();
                break;
            case 'r':
                detector->runTest();
                break;
            case 'o':
                detector->saveData();
                break;
            case 't':
                locator->print();
                break;
            case 's':
                if(display != nullptr) display->saveFrame();
                break;
            case 'l':
                if (!logging) {
                    logging = true;
                    std::cout << "Started logging\n";
                } else {
                    std::ofstream logFile;
                    logFile.open("log.csv", std::ios_base::app);
                    logFile << "FPS, Detections, PosX, PosY\n";
                    logFile << log;
                    logFile.close();
                    std::cout << "Saved log\n";
                }
                break;
            case 'm':
                if (takeMean) {
                    takeMean = false;
                    meanPos /= meanCounter;
                    std::cout << "Mean Pos - ";
                    meanPos.print();
                } else {
                    meanPos = {0, 0};
                    meanCounter = 0;
                    takeMean = true;
                    std::cout << "Taking mean\n";
                }
                
                break;
            default:
                break;
        }

        if (locator->newPos()) {

            if (takeMean) {
                meanPos += locator->getPos();;
                meanCounter++;
            }

            Point pos = locator->getPos();
            Pose tag = locator->getTagPose();
            ntCam->PutNumber("z", tag.getZin());
            ntCam->PutNumber("x", tag.getXin());
            ntCam->PutNumber("xPos", pos.x);
            ntCam->PutNumber("yPos", pos.y);
            ntCam->PutNumber("angle", tag.getAngle());
            ntCam->PutNumber("distance", tag.getDistance() * 39.3701);
            ntCam->PutBoolean("valid", true);
            ntCam->PutBoolean("stero", tag.getStero());
	        ntCam->PutNumber("id", tag.getId());
	        ntCam->PutNumber("heartbeat", heartbeat);
            inst.Flush();

	        heartbeat++;

            if (logging) {
                log += std::to_string(fps) + ", " + 
                std::to_string(detector->getDetectionsSize()) + ", " + 
                std::to_string(pos.x) + ", " + 
                std::to_string(pos.y) + "\n";
            }
        }

        if (display != nullptr) {
            display->setFrame(detector->getFrame());
            display->drawDetections(detector->getDetections());
        }

        if (stream != nullptr) {
            stream->writeFrame(detector->getFrame());
        }

        detector->destroyDetections();

        timer.stop();

        sum += timer.getTimeSec();
        every++;
        if (every == fpsDisplay) {
            fps = fpsDisplay / sum;
            if (display != nullptr) {display->setFps(fps);}
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


