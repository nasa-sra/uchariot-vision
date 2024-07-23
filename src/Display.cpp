#include "Display.h"
#include <iomanip>

Display::Display(int width, int height) : _frame() {

    _fps = 0.0;

    _width = width;
    _height = height;

    _imageIdx = 0;
    std::fstream idxFile;
    std::string idxFilePath = "./images/idx";
    idxFile.open(idxFilePath, std::ios::in);
    if (idxFile.is_open()) {
        idxFile >> _imageIdx;
    }
    idxFile.close();

    cv::namedWindow("Vision");

}

void Display::showFrame() {
    cv::Mat outFrame;
    cv::resize(_frame, outFrame, cv::Size(_width, _height));

    std::stringstream fpsText; 
    fpsText << "FPS: " << std::fixed << std::setprecision(1) << _fps;
    cv::putText(outFrame, fpsText.str(), cv::Point(20, 40), fontface, fontscale, cv::Scalar(0x00, 0x00, 0xff));

    const int crossSize = 10;
    cv::line(outFrame, cv::Point(_width / 2 - crossSize, _height / 2), cv::Point(_width / 2 + crossSize, _height / 2), cv::Scalar(0, 0, 0xff));
    cv::line(outFrame, cv::Point(_width / 2, _height / 2 - crossSize), cv::Point(_width / 2, _height / 2 + crossSize), cv::Scalar(0, 0, 0xff));

    cv::imshow("Vision", outFrame);
}

void Display::saveFrame() {
    std::string imageName = "./images/image" + std::to_string(_imageIdx) + ".jpg";
    cv::imwrite(imageName, _frame);
    std::cout << "Saved image to " << imageName << std::endl;
    _imageIdx++;

    std::fstream idxFile;
    std::string idxFilePath = "./images/idx";
    idxFile.open(idxFilePath, std::ofstream::out | std::ofstream::trunc);
    idxFile << _imageIdx;
    idxFile.close();
}