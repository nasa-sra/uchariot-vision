#pragma once

#include <string>
#include <Eigen/Core>

#include "Utils.h"

class Detection {
public:
    std::string name;
    Eigen::Vector3d pos;
    int x, y; // center

    std::string toJsonStr() {return Utils::StrFmt("{\"name\":\"%s\",\"x\":%f,\"y\":%f,\"z\":%f}", name, pos[0], pos[1], pos[2]);}
};

class ObjectDetection : public Detection {
public:
    float x1, y1, x2, y2; // corners
    float score;
};