
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>

class Display {
public:

    Display(int width, int height);

    void showFrame();
    void saveFrame();

    void setFrame(cv::Mat frame) { _frame = frame; }
    void setFps(float fps) { _fps = fps; }

    bool isOpen() {return cv::getWindowProperty("Vision", 0) >= 0; } // doesn't actually work

private:

    const int fontface = cv::FONT_HERSHEY_SIMPLEX;
    const double fontscale = 1.0;

    cv::Mat _frame;

    float _fps;

    int _width;
    int _height;

    int _imageIdx;

};