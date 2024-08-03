
#include "Detection.h"

cv::Scalar drawColor(0, 255, 0);

void Detection::draw(cv::Mat frame) {
    cv::circle(frame, cv::Point(x, y), 10, drawColor, 5);
    putText(frame, name, cv::Point2i(x - 15, y - 10), cv::FONT_HERSHEY_SIMPLEX, 1.0, drawColor, 3);
}
std::string Detection::toJsonStr() {
    return Utils::StrFmt("{\"name\":\"%s\",\"x\":%f,\"y\":%f,\"z\":%f}", name, pos[0], pos[1], pos[2]);
}

void ObjectDetection::draw(cv::Mat frame) {
    cv::rectangle(frame, box, drawColor, 3);
    std::string label = name + " - " + std::to_string(int(confidence * 100) / 100.0);
    cv::putText(frame, label.c_str() , cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.7, drawColor, 2);
}

std::string ObjectDetection::toJsonStr() {
    std::string pre = Detection::toJsonStr();
    return pre.substr(0, pre.size() - 1) + Utils::StrFmt(",\"confidence\":%f,\"width\":%f,\"height\":%f}", confidence, width, height);
}