#pragma once

#include <Eigen/Core>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include <tensorflow/lite/model.h>

#include "CameraBase.h"
#include "Detection.h"

class Detector {
public:
    Detector(CameraBase *camera);
    virtual std::vector<Detection> run() = 0;

protected:
    CameraBase *_camera;
};

class ClosestDetector : public Detector {
public:
    ClosestDetector(CameraBase *camera) : Detector(camera) {}
    void run(std::vector<Detection>& detections) override;
};

class ObjectDetector : public Detector {
public:
    ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath);
    void run(std::vector<Detection>& detections) override;

private:
    bool readFileContents(std::string fileName, std::vector<std::string>& lines);

    std::unique_ptr<tflite::FlatBufferModel> _model;
    std::unique_ptr<tflite::Interpreter> _interpreter;
    std::shared_ptr<edgetpu::EdgeTpuContext> _edgetpu_context;
    std::vector<std::string> _labels;

    double _confidenceThresh;
};
