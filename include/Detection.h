#pragma once

#include <string>
#include <Eigen/Core>

#include "Utils.h"

class Detection {
public:
    std::string name;
    Eigen::Vector3d pos;
    int pixelX, pixelY;

    std::string toJsonStr() {return Utils::StrFmt("{\"name\": \"%s\",\"pos\":[%f,%f,%f]}", name, pos[0], pos[1], pos[2]);}
};