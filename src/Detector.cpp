
#include "Detector.h"

Detector::Detector(CameraBase *camera)
{
    _camera = camera;
}

std::vector<Detection> ClosestDetector::run()
{
    cv::Mat colorFrame = _camera->getFrame();
    cv::Mat depthFrame = _camera->getDepthMap();

    // cv::Mat decimatedDepthFrame;
    // cv::resize(depthFrame, decimatedDepthFrame, cv::Size(), 0.25, 0.25);

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

    std::vector<Detection> detections;
    detections.push_back(det);
    return detections;
}