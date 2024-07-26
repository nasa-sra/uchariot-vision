
#include "Detector.h"

Detector::Detector(CameraBase *camera)
{
    _camera = camera;
}

std::vector<Detection> ClosestDetector::run(rapidjson::Document *doc)
{
    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    cv::Mat decimatedDepthFrame;
    cv::resize(depthFrame, decimatedDepthFrame, cv::Size(), 0.25, 0.25);

    float closest = 1e6;
    int x, y;
    for (int row = 1; row < depthFrame.rows / 2; row++)
    {
        for (int col = 1; col < depthFrame.cols; col++)
        {
            float pixel = depthFrame.at<float>(row, col);
            if (pixel > 0 && pixel < closest)
            {
                closest = pixel;
                x = col;
                y = row;
            }
        }
    }

    Detection det;
    det.pos = _camera->getCameraPoint(x, y);
    det.name = "closest";
    det.pixelX = x;
    det.pixelY = y;

    rapidjson::Value detectionResult(kObjectType);

    detectionResult.PushBack("poseX", det.pos.x, detectionResult->GetAllocator());
    detectionResult.PushBack("poseY", det.pos.y, detectionResult->GetAllocator());
    detectionResult.PushBack("poseZ", det.pos.z, detectionResult->GetAllocator());

    detectionResult.AddMember("name", det.name.c_str(), detectionResult->GetAllocator());

    doc->AddMember("detectionResult", detectionResult, doc->GetAllocator());

    rapidjson::Value

        std::vector<Detection>
            detections;
    detections.push_back(det);
    return detections;
}